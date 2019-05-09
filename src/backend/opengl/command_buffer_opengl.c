//
// Created by sandy on 20/04/19.
//

#include "command_buffer_opengl.h"

#include <assert.h>
#include <string.h>

#include <SDL_opengl.h>

#include "buffer_opengl.h"
#include "commands_opengl.h"
#include "context_opengl.h"
#include "framebuffer_opengl.h"
#include "pipeline_opengl.h"
#include "render_pass_opengl.h"
#include "vao_map.h"

bool
scegfx_command_buffer_opengl_initialize(scegfx_command_buffer_t* super)
{
  assert(!super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  assert(!this->has_begun);
  assert(!this->has_ended);

  super->initialized = true;
  return true;
}

void
scegfx_command_buffer_opengl_terminate(scegfx_command_buffer_t* super)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  glDeleteBuffers(this->vao_count, this->vaos);
  memset(&this->vao_map, 0, sizeof(this->vao_map));
  this->vao_count = 0;
  super->initialized = false;
}

bool
scegfx_command_buffer_opengl_begin(scegfx_command_buffer_t* super,
                                   bool long_lived)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;

  if (this->has_ended) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to begin command buffer: it has already begun and ended");
    return false;
  }

  if (this->has_begun) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to begin command buffer: it has already begun");
    return false;
  }

  this->has_begun = true;
  this->long_lived = long_lived;

  return true;
}

bool
scegfx_command_buffer_opengl_end(scegfx_command_buffer_t* super)
{

  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;

  if (!this->has_begun) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to end command buffer: it has ynotet begun");
    return false;
  }

  if (this->has_ended) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to end command buffer: it has already ended");
    return false;
  }

  this->has_ended = true;

  return true;
}

void
scegfx_command_buffer_opengl_begin_render_pass(
  scegfx_command_buffer_t* super,
  const scegfx_render_pass_begin_info_t* info)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;

  if (this->has_ended) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to encode command: command buffer begun and ended");
    return;
  }

  if (!this->has_begun) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to encode command: command buffer has not begun");
    return;
  }

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);

  this->commands[this->count] = scegfx_command_begin_render_pass_opengl;
  this->args[this->count].render_pass_begin_info = *info;

  ++this->count;
}

void
scegfx_command_buffer_opengl_end_render_pass(scegfx_command_buffer_t* super)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;

  if (this->has_ended) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to encode command: command buffer begun and ended");
    return;
  }

  if (!this->has_begun) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to encode command: command buffer has not begun");
    return;
  }

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);

  this->commands[this->count] = scegfx_command_end_render_pass_opengl;

  ++this->count;
}

void
scegfx_command_buffer_opengl_bind_pipeline(scegfx_command_buffer_t* super,
                                           scegfx_pipeline_type_t type,
                                           const scegfx_pipeline_t* pipeline)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_pipeline_opengl_t* pipeline_opengl =
    (scegfx_pipeline_opengl_t*)pipeline;
  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);
  assert(type == pipeline_opengl->type);
#if defined(EMSCRIPTEN)
  assert(pipeline_opengl->type == scegfx_pipeline_type_graphics ||
         pipeline_opengl->type == scegfx_pipeline_type_compute);
#else
  assert(pipeline_opengl->type == scegfx_pipeline_type_graphics);
#endif

  this->commands[this->count] = scegfx_command_opengl_bind_pipeline;
  this->args[this->count].bind_pipeline.type = pipeline_opengl->type;
  this->args[this->count].bind_pipeline.program = pipeline_opengl->program;
  if (pipeline_opengl->type == scegfx_pipeline_type_graphics) {
    this->draw_mode = pipeline_opengl->graphics.topology;
    assert(pipeline_opengl->graphics.attribute_desc_count <=
           sizeof(this->vao_desc.attributes) /
             sizeof(this->vao_desc.attributes[0]));
    for (uint32_t i = 0; i < pipeline_opengl->graphics.attribute_desc_count;
         ++i) {
      uint32_t size = 0;
      uint32_t format = 0;
      bool normalized = false;
      switch (pipeline_opengl->graphics.attribute_descs[i].format) {
        case scegfx_format_r32g32_sfloat:
          size = 2;
          format = 0x1406 /*GL_FLOAT*/;
          normalized = false;
          break;
        case scegfx_format_r32g32b32_sfloat:
          size = 3;
          format = 0x1406 /*GL_FLOAT*/;
          normalized = false;
          break;
        case scegfx_format_r32g32b32a32_sfloat:
          size = 4;
          format = 0x1406 /*GL_FLOAT*/;
          normalized = false;
          break;
        default:
          assert(false);
      }
      this->vao_desc.attributes[i].location =
        pipeline_opengl->graphics.attribute_descs[i].location;
      this->vao_desc.attributes[i].size = size;
      this->vao_desc.attributes[i].format = format;
      this->vao_desc.attributes[i].normalized = normalized;
      this->vao_desc.attributes[i].offset =
        pipeline_opengl->graphics.attribute_descs[i].offset;
    }
    this->vao_desc.attribute_count =
      pipeline_opengl->graphics.attribute_desc_count;
    this->vao_desc.vertex.stride =
      pipeline_opengl->graphics.binding_desc.stride;
  }

  if (pipeline_opengl->type == scegfx_pipeline_type_graphics) {
    this->args[this->count].bind_pipeline.graphics.front_face =
      pipeline_opengl->graphics.front_face;
    this->args[this->count].bind_pipeline.graphics.cull_face =
      pipeline_opengl->graphics.cull_face;
    this->args[this->count].bind_pipeline.graphics.line_width =
      pipeline_opengl->graphics.line_width;
  }

  ++this->count;
}

void
scegfx_command_buffer_opengl_bind_vertex_buffer(scegfx_command_buffer_t* super,
                                                const scegfx_buffer_t* buffer,
                                                scegfx_device_size_t offset)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_buffer_opengl_t* buffer_gl = (scegfx_buffer_opengl_t*)buffer;

  this->vao_desc.vertex.handle = buffer_gl->handle;
  this->vao_desc.vertex.offset = offset;
}

void
scegfx_command_buffer_opengl_bind_index_buffer(scegfx_command_buffer_t* super,
                                               const scegfx_buffer_t* buffer,
                                               scegfx_device_size_t offset,
                                               scegfx_index_type_t index_type)
{

  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_buffer_opengl_t* buffer_gl = (scegfx_buffer_opengl_t*)buffer;

  this->vao_desc.index.handle = buffer_gl->handle;
  this->vao_desc.index.offset = offset;

  this->index_type = index_type;
}

void
scegfx_command_buffer_opengl_resolve_vao(scegfx_command_buffer_opengl_t* this)
{
  bool create = true;
  uint32_t vao = 0;
  {
    uint8_t out_vao = 0;
    bool found = scegfx_vao_map_get(&this->vao_map, &this->vao_desc, &out_vao);
    vao = out_vao;
    if (found) {
      create = false;
    }
  }
  if (create) {
    glGenVertexArrays(1, &vao);
    this->vaos[this->vao_count++] = vao;
    glBindVertexArray(vao);

    if (this->vao_desc.index.handle) {
      scegfx_device_size_t stride = 0;
      switch (this->index_type) {
        case scegfx_index_type_u16:
          stride = sizeof(uint16_t);
          break;
        case scegfx_index_type_u32:
          stride = sizeof(uint32_t);
          break;
        default:
          assert(false);
          break;
      }
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vao_desc.index.handle);
      //    glBindBufferRange(GL_ELEMENT_ARRAY_BUFFER,
      //                      0,
      //                      this->vao_desc.index.handle,
      //                      (intptr_t)this->vao_desc.index.offset,
      //                      stride);
    }

#if defined(EMSCRIPTEN)
    glBindBuffer(GL_ARRAY_BUFFER, this->vao_desc.vertex.handle);
    //  glBindBufferRange(GL_ARRAY_BUFFER,
    //                    0,
    //                    this->vao_desc.vertex.handle,
    //                    (intptr_t)this->vao_desc.vertex.offset,
    //                    this->vao_desc.vertex.stride);
#else
    glBindVertexBuffer(0,
                       this->vao_desc.vertex.handle,
                       (intptr_t)this->vao_desc.vertex.offset,
                       this->vao_desc.vertex.stride);
#endif // defined(EMSCRIPTEN)

    assert(this->vao_desc.attribute_count <=
           sizeof(this->vao_desc.attributes) /
             sizeof(this->vao_desc.attributes[0]));
    for (uint32_t i = 0; i < this->vao_desc.attribute_count; ++i) {
      glEnableVertexAttribArray(this->vao_desc.attributes[i].location);
#if defined(EMSCRIPTEN)
      glVertexAttribPointer(
        this->vao_desc.attributes[i].location,
        this->vao_desc.attributes[i].size,
        this->vao_desc.attributes[i].format,
        this->vao_desc.attributes[i].normalized,
        this->vao_desc.vertex.stride,
        (void*)(uintptr_t)this->vao_desc.attributes[i].offset);
      assert(this->vao_desc.attributes[i].binding == 0);
#else
      glVertexAttribFormat(this->vao_desc.attributes[i].location,
                           this->vao_desc.attributes[i].size,
                           this->vao_desc.attributes[i].format,
                           this->vao_desc.attributes[i].normalized,
                           this->vao_desc.attributes[i].offset);
      glVertexAttribBinding(this->vao_desc.attributes[i].location,
                            this->vao_desc.attributes[i].binding);
#endif // defined(EMSCRIPTEN)
    }

    glBindVertexArray(0);
    scegfx_vao_map_add(&this->vao_map, &this->vao_desc, vao);
  }

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);
  this->commands[this->count] = scegfx_command_opengl_bind_vao;
  this->args[this->count].bind_vao.vertex_array_object = vao;
  ++this->count;
}

void
scegfx_command_buffer_opengl_draw(scegfx_command_buffer_t* super,
                                  uint32_t vertex_count,
                                  uint32_t instance_count,
                                  uint32_t first_vertex,
                                  uint32_t first_instance)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;

  scegfx_command_buffer_opengl_resolve_vao(this);

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);

  this->commands[this->count] = scegfx_command_opengl_draw;
  this->args[this->count].draw.mode = this->draw_mode;
  this->args[this->count].draw.vertex_count = vertex_count;
  this->args[this->count].draw.instance_count = instance_count;
  this->args[this->count].draw.first_vertex = first_vertex;
  this->args[this->count].draw.first_instance = first_instance;
  ++this->count;
}

void
scegfx_command_buffer_opengl_draw_indexed(scegfx_command_buffer_t* super,
                                          uint32_t index_count,
                                          uint32_t instance_count,
                                          uint32_t first_index,
                                          int32_t vertex_offset,
                                          uint32_t first_instance)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;

  scegfx_command_buffer_opengl_resolve_vao(this);

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);

  uint32_t index_type = 0x1405 /*GL_UNSIGNED_INT*/;
  switch (this->index_type) {
    case scegfx_index_type_u16:
      index_type = 0x1403 /*GL_UNSIGNED_SHORT*/;
      break;
    case scegfx_index_type_u32:
      index_type = 0x1405 /*GL_UNSIGNED_INT*/;
      break;
    default:
      context->super.debug_callback(
        scegfx_debug_severity_error,
        __LINE__,
        FILE_BASENAME,
        "Unable to encode command: invalid index type");
      return;
  }

  this->commands[this->count] = scegfx_command_opengl_draw_indexed;
  this->args[this->count].draw_indexed.mode = this->draw_mode;
  this->args[this->count].draw_indexed.index_type = index_type;
  this->args[this->count].draw_indexed.index_count = index_count;
  this->args[this->count].draw_indexed.instance_count = instance_count;
  this->args[this->count].draw_indexed.first_index = first_index;
  this->args[this->count].draw_indexed.vertex_offset = vertex_offset;
  this->args[this->count].draw_indexed.first_instance = first_instance;

  ++this->count;
}

void
scegfx_command_buffer_opengl_debug_marker_begin(
  scegfx_command_buffer_t* super,
  const scegfx_debug_marker_info_t* info)
{
  assert(super->initialized);
  assert(info);
#if !defined(EMSCRIPTEN)
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);

  this->commands[this->count] = scegfx_command_opengl_debug_marker_begin;
  this->args[this->count].debug_maker_info = *info;

  ++this->count;
#endif
}

void
scegfx_command_buffer_opengl_debug_marker_end(scegfx_command_buffer_t* super)
{
  assert(super->initialized);
#if !defined(EMSCRIPTEN)
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);

  this->commands[this->count] = scegfx_command_opengl_debug_marker_end;

  ++this->count;
#endif
}

bool
scegfx_command_buffer_opengl_execute(scegfx_command_buffer_opengl_t* this)
{
  scegfx_context_opengl_t* context =
    (scegfx_context_opengl_t*)this->super.context;
  if (!this->long_lived && this->executed_once) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to execute command buffer: short "
                                  "lived command buffer already run");
    return false;
  }

  if (!this->has_begun) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to execute command buffer: not started");
    return false;
  }

  if (!this->has_ended) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to execute command buffer: not ended");
    return false;
  }

  for (uint32_t i = 0; i < this->count; ++i) {
    this->commands[i](&this->args[i]);
  }

  this->executed_once = true;
  return true;
}

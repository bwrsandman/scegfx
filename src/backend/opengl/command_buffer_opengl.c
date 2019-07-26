//
// Created by sandy on 20/04/19.
//

#include "command_buffer_opengl.h"

#include <assert.h>
#include <string.h>

#include <SDL_opengl.h>

#include <scegfx/device_memory.h>

#include "buffer_opengl.h"
#include "commands_opengl.h"
#include "context_opengl.h"
#include "descriptor_set_opengl.h"
#include "framebuffer_opengl.h"
#include "image_opengl.h"
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void
scegfx_command_buffer_opengl_bind_descriptor_set(
  scegfx_command_buffer_t* super,
  scegfx_pipeline_type_t bind_point,
  const scegfx_pipeline_layout_t* layout,
  const scegfx_descriptor_set_t* descriptor_set)
#pragma clang diagnostic pop
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  const scegfx_descriptor_set_opengl_t* descriptor_set_gl =
    (const scegfx_descriptor_set_opengl_t*)descriptor_set;

  for (uint32_t i = 0; i < descriptor_set_gl->descriptor_count; ++i) {
    assert(this->count + 1 < SCEGFX_MAX_COMMANDS);
    const scegfx_descriptor_t* descriptor = &descriptor_set_gl->descriptors[i];

    switch (descriptor->type) {
      case scegfx_descriptor_type_sampled_image: {
        this->commands[this->count] =
          scegfx_command_opengl_bind_descriptor_set_sampled_image;
        switch (descriptor->sampled_image.image.type) {
          case scegfx_image_type_1d:
            this->args[this->count].bind_descriptor_set_sampled_image.target =
              GL_TEXTURE_1D;
            break;
          case scegfx_image_type_2d:
            this->args[this->count].bind_descriptor_set_sampled_image.target =
              GL_TEXTURE_2D;
            break;
          case scegfx_image_type_3d:
            this->args[this->count].bind_descriptor_set_sampled_image.target =
              GL_TEXTURE_3D;
            break;
          default:
            assert(false);
            break;
        }
        this->args[this->count].bind_descriptor_set_sampled_image.texture_unit =
          GL_TEXTURE0 + descriptor->index;
        this->args[this->count].bind_descriptor_set_sampled_image.texture =
          descriptor->sampled_image.image.handle;
        this->args[this->count].bind_descriptor_set_sampled_image.mag_filter =
          descriptor->sampled_image.sampler.mag_filter;
        this->args[this->count].bind_descriptor_set_sampled_image.min_filter =
          descriptor->sampled_image.sampler.min_filter;
        this->args[this->count]
          .bind_descriptor_set_sampled_image.texture_wrap_s =
          descriptor->sampled_image.sampler.texture_wrap_s;
        this->args[this->count]
          .bind_descriptor_set_sampled_image.texture_wrap_t =
          descriptor->sampled_image.sampler.texture_wrap_t;
        this->args[this->count]
          .bind_descriptor_set_sampled_image.texture_wrap_r =
          descriptor->sampled_image.sampler.texture_wrap_r;
      } break;
      case scegfx_descriptor_type_uniform_buffer: {
        this->commands[this->count] =
          scegfx_command_opengl_bind_descriptor_set_uniform_buffer;
        this->args[this->count].bind_descriptor_set_uniform_buffer.index =
          descriptor->index;
        this->args[this->count].bind_descriptor_set_uniform_buffer.buffer =
          descriptor->buffer.handle;
        this->args[this->count].bind_descriptor_set_uniform_buffer.offset =
          descriptor->buffer.offset;
        this->args[this->count].bind_descriptor_set_uniform_buffer.size =
          descriptor->buffer.range;
      } break;
      default:
        assert(false);
        break;
    }

    ++this->count;
  }
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
scegfx_command_buffer_opengl_copy_buffer(scegfx_command_buffer_t* super,
                                         const scegfx_buffer_t* src_buffer,
                                         const scegfx_buffer_t* dst_buffer,
                                         const scegfx_buffer_copy_t* region)
{
  assert(super->initialized);
  assert(src_buffer->initialized);
  assert(dst_buffer->initialized);
  assert(region);

  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_buffer_opengl_t* src_buffer_gl = (scegfx_buffer_opengl_t*)src_buffer;
  scegfx_buffer_opengl_t* dst_buffer_gl = (scegfx_buffer_opengl_t*)dst_buffer;

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);

  this->commands[this->count] = scegfx_command_opengl_copy_buffer;
  this->args[this->count].copy_buffer.src_buffer_handle = src_buffer_gl->handle;
  this->args[this->count].copy_buffer.src_target = GL_COPY_READ_BUFFER;
  this->args[this->count].copy_buffer.dst_buffer_handle = dst_buffer_gl->handle;
  this->args[this->count].copy_buffer.dst_target = GL_COPY_WRITE_BUFFER;
  this->args[this->count].copy_buffer.region = *region;

  ++this->count;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
void
scegfx_command_buffer_opengl_copy_image(scegfx_command_buffer_t* super,
                                        const scegfx_image_t* src_image,
                                        scegfx_image_layout_t src_image_layout,
                                        const scegfx_image_t* dst_image,
                                        scegfx_image_layout_t dst_image_layout,
                                        const scegfx_image_copy_t* region)
{
  assert(super->initialized);
  assert(src_image->initialized);
  assert(dst_image->initialized);
  assert(src_image->type == dst_image->type);
  assert(region);
  assert(region->src_subresource.mip_level ==
         region->dst_subresource.mip_level);

  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_image_opengl_t* src_image_gl = (scegfx_image_opengl_t*)src_image;
  scegfx_image_opengl_t* dst_image_gl = (scegfx_image_opengl_t*)dst_image;

#if defined(EMSCRIPTEN)
  switch (src_image_gl->super.type) {
    case scegfx_image_type_2d:
      this->commands[this->count] = scegfx_command_opengl_copy_texture_2d;
      break;
    case scegfx_image_type_3d:
      this->commands[this->count] = scegfx_command_opengl_copy_texture_3d;
      break;
    default:
      assert(false);
      return;
  }
#else
  this->commands[this->count] = scegfx_command_opengl_copy_image;
#endif // defined(EMSCRIPTEN)
  this->args[this->count].copy_image.src_image_handle = src_image_gl->handle;
  this->args[this->count].copy_image.dst_image_handle = dst_image_gl->handle;
  this->args[this->count].copy_image.region = *region;

  ++this->count;
}

void
scegfx_command_buffer_opengl_blit_image(scegfx_command_buffer_t* super,
                                        const scegfx_image_t* src_image,
                                        scegfx_image_layout_t src_image_layout,
                                        const scegfx_image_t* dst_image,
                                        scegfx_image_layout_t dst_image_layout,
                                        const scegfx_image_blit_t* region)
{
  assert(super->initialized);
  assert(src_image->initialized);
  assert(dst_image->initialized);
  assert(region);

  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_image_opengl_t* src_image_gl = (scegfx_image_opengl_t*)src_image;
  scegfx_image_opengl_t* dst_image_gl = (scegfx_image_opengl_t*)dst_image;

  assert(false);
}

void
scegfx_command_buffer_opengl_copy_buffer_to_image(
  scegfx_command_buffer_t* super,
  const scegfx_buffer_t* buffer,
  const scegfx_image_t* image,
  scegfx_image_layout_t image_layout,
  const scegfx_buffer_image_copy_t* region)
{
  assert(super->initialized);
  assert(buffer->initialized);
  assert(image->initialized);
  assert(region);

  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_buffer_opengl_t* buffer_gl = (scegfx_buffer_opengl_t*)buffer;
  scegfx_image_opengl_t* image_gl = (scegfx_image_opengl_t*)image;

  assert(region->image_offset.x == 0);
  assert(region->image_offset.y == 0);
  assert(region->image_offset.z == 0);
  scegfx_device_size_t dst_offset = 0;

  assert(region->image_extent.width == image->extent.width);
  assert(region->image_extent.height == image->extent.height);
  assert(region->image_extent.depth == 1);

  scegfx_device_memory_requirements_t memory_requirements;
  super->context->api_vtable->get_image_memory_requirements(
    super->context, image, &memory_requirements);

  // Copy buffer to buffer texture
  scegfx_buffer_copy_t buffer_region = {
    .src_offset = region->buffer_offset,
    .dst_offset = dst_offset,
    .size = memory_requirements.size,
  };
  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);
  assert(buffer_gl->handle);
  assert(image_gl->handle);

  switch (image_gl->super.type) {
    case scegfx_image_type_1d:
#if defined(EMSCRIPTEN)
      assert(false);
      return;
#else
      this->commands[this->count] =
        scegfx_command_opengl_copy_buffer_to_image_1d;
      break;
#endif
    case scegfx_image_type_2d:
      this->commands[this->count] =
        scegfx_command_opengl_copy_buffer_to_image_2d;
      break;
    case scegfx_image_type_3d:
      this->commands[this->count] =
        scegfx_command_opengl_copy_buffer_to_image_3d;
      break;
    default:
      assert(false);
      return;
  }

  this->args[this->count].copy_buffer_to_image.buffer_handle =
    buffer_gl->handle;
  this->args[this->count].copy_buffer_to_image.image_handle = image_gl->handle;
  this->args[this->count].copy_buffer_to_image.image_target = image_gl->target;
  this->args[this->count].copy_buffer_to_image.image_format = GL_RGBA;
  this->args[this->count].copy_buffer_to_image.image_type = GL_UNSIGNED_BYTE;
  this->args[this->count].copy_buffer_to_image.region = *region;

  ++this->count;
}

void
scegfx_command_buffer_opengl_copy_image_to_buffer(
  scegfx_command_buffer_t* super,
  const scegfx_image_t* image,
  scegfx_image_layout_t image_layout,
  const scegfx_buffer_t* buffer,
  const scegfx_buffer_image_copy_t* region)
{
  assert(super->initialized);
  assert(image->initialized);
  assert(buffer->initialized);
  assert(region);

  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;
  scegfx_buffer_opengl_t* buffer_gl = (scegfx_buffer_opengl_t*)buffer;
  scegfx_image_opengl_t* image_gl = (scegfx_image_opengl_t*)image;

  assert(region->image_offset.x == 0);
  assert(region->image_offset.y == 0);
  assert(region->image_offset.z == 0);
  scegfx_device_size_t src_offset = 0;

  assert(region->image_extent.width == image->extent.width);
  assert(region->image_extent.height == image->extent.height);
  assert(region->image_extent.depth == 1);

  scegfx_device_memory_requirements_t memory_requirements;
  super->context->api_vtable->get_image_memory_requirements(
    super->context, image, &memory_requirements);

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);
  assert(image_gl->handle);
  this->commands[this->count] = scegfx_command_opengl_copy_image_to_buffer;
  ++this->count;
}

void
scegfx_command_buffer_opengl_pipeline_barrier(
  scegfx_command_buffer_t* super,
  scegfx_pipeline_stage_t src_stage_mask,
  scegfx_pipeline_stage_t dst_stage_mask,
  uint32_t image_memory_barrier_count,
  const scegfx_image_memory_barrier_t* image_memory_barriers)
{
  assert(super->initialized);
  assert(image_memory_barriers);
#if !defined(EMSCRIPTEN)
  scegfx_command_buffer_opengl_t* this = (scegfx_command_buffer_opengl_t*)super;

  assert(this->count + 1 < SCEGFX_MAX_COMMANDS);

  GLbitfield barriers = 0;
  if (image_memory_barrier_count) {
    barriers |= GL_TEXTURE_UPDATE_BARRIER_BIT;
  }

  this->commands[this->count] = scegfx_command_opengl_memory_barrier;
  this->args[this->count].memory_barrier.barriers = barriers;

  ++this->count;
#endif
}
#pragma clang diagnostic pop

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

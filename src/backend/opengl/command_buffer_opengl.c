//
// Created by sandy on 20/04/19.
//

#include "command_buffer_opengl.h"

#include <assert.h>
#include <string.h>

#include "commands_opengl.h"
#include "context_opengl.h"
#include "framebuffer_opengl.h"
#include "pipeline_opengl.h"
#include "render_pass_opengl.h"

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
    this->args[this->count].bind_pipeline.graphics.front_face =
      pipeline_opengl->graphics.front_face;
    this->args[this->count].bind_pipeline.graphics.cull_face =
      pipeline_opengl->graphics.cull_face;
    this->args[this->count].bind_pipeline.graphics.line_width =
      pipeline_opengl->graphics.line_width;
  }

  ++this->count;
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

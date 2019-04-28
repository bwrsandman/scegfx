//
// Created by sandy on 20/04/19.
//

#include "command_buffer_opengl.h"

#include <assert.h>
#include <string.h>

#include "context_opengl.h"
#include "framebuffer_opengl.h"
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

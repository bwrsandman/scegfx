//
// Created by sandy on 20/04/19.
//

#ifndef SCEGFX_COMMAND_BUFFER_OPENGL_H
#define SCEGFX_COMMAND_BUFFER_OPENGL_H

#include <scegfx/command_buffer.h>

#include "commands_opengl.h"

enum
{
  SCEGFX_MAX_COMMANDS = 0x20,
};

typedef struct scegfx_command_buffer_opengl_t
{
  scegfx_command_buffer_t super;
  bool has_begun;
  bool has_ended;
  bool long_lived;
  bool executed_once;
  uint32_t count;
  scegfx_command_opengl_t commands[SCEGFX_MAX_COMMANDS];
  scegfx_command_arg_t args[SCEGFX_MAX_COMMANDS];
} scegfx_command_buffer_opengl_t;

bool
scegfx_command_buffer_opengl_initialize(scegfx_command_buffer_t* this);
void
scegfx_command_buffer_opengl_terminate(scegfx_command_buffer_t* this);

bool
scegfx_command_buffer_opengl_begin(scegfx_command_buffer_t* this,
                                   bool long_lived);
bool
scegfx_command_buffer_opengl_end(scegfx_command_buffer_t* this);

void
scegfx_command_buffer_opengl_begin_render_pass(
  scegfx_command_buffer_t* this,
  const scegfx_render_pass_begin_info_t* info);
void
scegfx_command_buffer_opengl_end_render_pass(scegfx_command_buffer_t* this);

void
scegfx_command_buffer_opengl_bind_pipeline(scegfx_command_buffer_t* this,
                                           scegfx_pipeline_type_t type,
                                           const scegfx_pipeline_t* pipeline);

const static scegfx_command_buffer_api_vtable_t
  scegfx_command_buffer_api_vtable_opengl = {
    .initialize = scegfx_command_buffer_opengl_initialize,
    .terminate = scegfx_command_buffer_opengl_terminate,
    .begin = scegfx_command_buffer_opengl_begin,
    .end = scegfx_command_buffer_opengl_end,
    .begin_render_pass = scegfx_command_buffer_opengl_begin_render_pass,
    .end_render_pass = scegfx_command_buffer_opengl_end_render_pass,
    .bind_pipeline = scegfx_command_buffer_opengl_bind_pipeline,
  };

bool
scegfx_command_buffer_opengl_execute(scegfx_command_buffer_opengl_t* this);

#endif // SCEGFX_COMMAND_BUFFER_OPENGL_H

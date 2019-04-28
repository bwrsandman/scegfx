//
// Created by sandy on 05/05/19.
//

#ifndef SCEGFX_COMMANDS_OPENGL_H
#define SCEGFX_COMMANDS_OPENGL_H

#include <scegfx/command_buffer.h>

typedef union scegfx_command_arg_t
{
  scegfx_render_pass_begin_info_t render_pass_begin_info;
} scegfx_command_arg_t;

typedef void (*scegfx_command_opengl_t)(const scegfx_command_arg_t* arg);

void
scegfx_command_begin_render_pass_opengl(const scegfx_command_arg_t* arg);
void
scegfx_command_end_render_pass_opengl(const scegfx_command_arg_t* arg);

#endif // SCEGFX_COMMANDS_OPENGL_H

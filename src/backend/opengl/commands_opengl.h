//
// Created by sandy on 05/05/19.
//

#ifndef SCEGFX_COMMANDS_OPENGL_H
#define SCEGFX_COMMANDS_OPENGL_H

#include <scegfx/command_buffer.h>

#include "pipeline_opengl.h"

typedef union scegfx_command_arg_t
{
  scegfx_render_pass_begin_info_t render_pass_begin_info;
  struct
  {
    uint32_t program;
    scegfx_pipeline_type_t type;
    union
    {
      struct
      {
        uint32_t front_face;
        uint32_t cull_face;
        float line_width;
      } graphics;
    };
  } bind_pipeline;
  struct
  {
    uint32_t vertex_array_object;
  } bind_vao;
  struct
  {
    uint32_t mode;
    uint32_t vertex_count;
    uint32_t instance_count;
    uint64_t first_vertex;
    uint32_t first_instance;
  } draw;
  struct
  {
    uint32_t mode;
    uint32_t index_count;
    uint32_t index_type;
    uint32_t instance_count;
    uint64_t first_index;
    int32_t vertex_offset;
    uint32_t first_instance;
  } draw_indexed;
  scegfx_debug_marker_info_t debug_maker_info;
} scegfx_command_arg_t;

typedef void (*scegfx_command_opengl_t)(const scegfx_command_arg_t* arg);

void
scegfx_command_begin_render_pass_opengl(const scegfx_command_arg_t* arg);
void
scegfx_command_end_render_pass_opengl(const scegfx_command_arg_t* arg);
void
scegfx_command_opengl_bind_pipeline(const scegfx_command_arg_t* arg);
void
scegfx_command_opengl_bind_vao(const scegfx_command_arg_t* arg);
void
scegfx_command_opengl_draw(const scegfx_command_arg_t* arg);
void
scegfx_command_opengl_draw_indexed(const scegfx_command_arg_t* arg);
void
scegfx_command_opengl_debug_marker_begin(const scegfx_command_arg_t* arg);
void
scegfx_command_opengl_debug_marker_end(const scegfx_command_arg_t* arg);

#endif // SCEGFX_COMMANDS_OPENGL_H

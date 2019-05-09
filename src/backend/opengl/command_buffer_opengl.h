//
// Created by sandy on 20/04/19.
//

#ifndef SCEGFX_COMMAND_BUFFER_OPENGL_H
#define SCEGFX_COMMAND_BUFFER_OPENGL_H

#include <scegfx/command_buffer.h>

#include "commands_opengl.h"
#include "vao_map.h"

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
  scegfx_index_type_t index_type;
  uint32_t draw_mode;
  scegfx_vao_map_t vao_map;
  uint32_t vao_count;
  uint32_t vaos[SCEGFX_VAO_MAP_BUFFER_SIZE];
  scegfx_vao_map_key_t vao_desc;
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
void
scegfx_command_buffer_opengl_bind_vertex_buffer(scegfx_command_buffer_t* super,
                                                const scegfx_buffer_t* buffer,
                                                scegfx_device_size_t offset);
void
scegfx_command_buffer_opengl_bind_index_buffer(
  scegfx_command_buffer_t* this,
  const scegfx_buffer_t* buffer,
  const scegfx_device_size_t offset,
  scegfx_index_type_t index_type);

void
scegfx_command_buffer_opengl_draw(scegfx_command_buffer_t* this,
                                  uint32_t vertex_count,
                                  uint32_t instance_count,
                                  uint32_t first_vertex,
                                  uint32_t first_instance);
void
scegfx_command_buffer_opengl_draw_indexed(scegfx_command_buffer_t* this,
                                          uint32_t index_count,
                                          uint32_t instance_count,
                                          uint32_t first_index,
                                          int32_t vertex_offset,
                                          uint32_t first_instance);

void
scegfx_command_buffer_opengl_debug_marker_begin(
  scegfx_command_buffer_t* this,
  const scegfx_debug_marker_info_t* info);
void
scegfx_command_buffer_opengl_debug_marker_end(scegfx_command_buffer_t* this);

const static scegfx_command_buffer_api_vtable_t
  scegfx_command_buffer_api_vtable_opengl = {
    .initialize = scegfx_command_buffer_opengl_initialize,
    .terminate = scegfx_command_buffer_opengl_terminate,
    .begin = scegfx_command_buffer_opengl_begin,
    .end = scegfx_command_buffer_opengl_end,
    .begin_render_pass = scegfx_command_buffer_opengl_begin_render_pass,
    .end_render_pass = scegfx_command_buffer_opengl_end_render_pass,
    .bind_pipeline = scegfx_command_buffer_opengl_bind_pipeline,
    .bind_vertex_buffer = scegfx_command_buffer_opengl_bind_vertex_buffer,
    .bind_index_buffer = scegfx_command_buffer_opengl_bind_index_buffer,
    .draw = scegfx_command_buffer_opengl_draw,
    .draw_indexed = scegfx_command_buffer_opengl_draw_indexed,
    .debug_marker_begin = scegfx_command_buffer_opengl_debug_marker_begin,
    .debug_marker_end = scegfx_command_buffer_opengl_debug_marker_end,
  };

bool
scegfx_command_buffer_opengl_execute(scegfx_command_buffer_opengl_t* this);

#endif // SCEGFX_COMMAND_BUFFER_OPENGL_H

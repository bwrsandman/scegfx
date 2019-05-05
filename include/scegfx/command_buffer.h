//
// Created by sandy on 20/04/19.
//

#ifndef SCEGFX_COMMAND_BUFFER_H
#define SCEGFX_COMMAND_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "pipeline.h"

typedef struct scegfx_command_buffer_t scegfx_command_buffer_t;
typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_framebuffer_t scegfx_framebuffer_t;
typedef struct scegfx_render_pass_t scegfx_render_pass_t;

typedef struct scegfx_render_pass_begin_info_t
{
  scegfx_rect_2d_t render_area;
  struct
  {
    float color[4];
    float depth;
  } clear_values;
  scegfx_render_pass_t* render_pass;
  scegfx_framebuffer_t* framebuffer;
} scegfx_render_pass_begin_info_t;

typedef struct scegfx_debug_marker_info_t
{
  const char* name;
  float color[4];
} scegfx_debug_marker_info_t;

typedef struct scegfx_command_buffer_api_vtable_t
{
  bool (*initialize)(scegfx_command_buffer_t* this);
  void (*terminate)(scegfx_command_buffer_t* this);

  bool (*begin)(scegfx_command_buffer_t* this, bool long_lived);
  bool (*end)(scegfx_command_buffer_t* this);

  void (*begin_render_pass)(scegfx_command_buffer_t* this,
                            const scegfx_render_pass_begin_info_t* info);
  void (*end_render_pass)(scegfx_command_buffer_t* this);

  void (*bind_pipeline)(scegfx_command_buffer_t* this,
                        scegfx_pipeline_type_t type,
                        const scegfx_pipeline_t* pipeline);

  void (*debug_marker_begin)(scegfx_command_buffer_t* this,
                             const scegfx_debug_marker_info_t* info);
  void (*debug_marker_end)(scegfx_command_buffer_t* this);
} scegfx_command_buffer_api_vtable_t;

typedef struct scegfx_command_buffer_t
{
  bool initialized;
  const scegfx_command_buffer_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_command_buffer_t;

#endif // SCEGFX_COMMAND_BUFFER_H

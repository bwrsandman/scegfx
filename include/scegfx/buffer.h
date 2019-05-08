//
// Created by sandy on 05/05/19.
//

#ifndef SCEGFX_BUFFER_H
#define SCEGFX_BUFFER_H

#include <stdbool.h>

#include "common.h"

typedef struct scegfx_buffer_t scegfx_buffer_t;
typedef struct scegfx_context_t scegfx_context_t;

typedef enum scegfx_buffer_usage_t
{
  scegfx_buffer_usage_transfer_src = 0x0001u,
  scegfx_buffer_usage_transfer_dst = 0x0002u,
  scegfx_buffer_usage_uniform_texel_buffer = 0x0004u,
  scegfx_buffer_usage_storage_texel_buffer = 0x0008u,
  scegfx_buffer_usage_uniform_buffer = 0x0010u,
  scegfx_buffer_usage_storage_buffer = 0x0020u,
  scegfx_buffer_usage_index_buffer = 0x0040u,
  scegfx_buffer_usage_vertex_buffer = 0x0080u,
  scegfx_buffer_usage_indirect_buffer = 0x0100u,
  scegfx_buffer_usage_conditional_rendering = 0x0200u,
  scegfx_buffer_usage_ray_tracing = 0x0400u,
  scegfx_buffer_usage_transform_feedback_buffer = 0x0800u,
  scegfx_buffer_usage_transform_feedback_counter_buffer = 0x1000u,
  scegfx_buffer_usage_shader_device_address = 0x20000u,
} scegfx_buffer_usage_t;

typedef struct scegfx_buffer_api_vtable_t
{
  bool (*initialize)(scegfx_buffer_t* this,
                     scegfx_device_size_t size,
                     scegfx_buffer_usage_t usage);
  void (*terminate)(scegfx_buffer_t* this);
} scegfx_buffer_api_vtable_t;

typedef struct scegfx_buffer_t
{
  bool initialized;
  const scegfx_buffer_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_buffer_t;

#endif // SCEGFX_BUFFER_H

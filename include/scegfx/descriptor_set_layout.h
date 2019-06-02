//
// Created by sandy on 31/05/19.
//

#ifndef SCEGFX_DESCRIPTOR_SET_LAYOUT_H
#define SCEGFX_DESCRIPTOR_SET_LAYOUT_H

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

typedef struct scegfx_descriptor_set_layout_t scegfx_descriptor_set_layout_t;
typedef struct scegfx_context_t scegfx_context_t;

typedef enum scegfx_descriptor_type_t
{
  scegfx_descriptor_type_sampler = 0,
  scegfx_descriptor_type_image_sampler = 1,
  scegfx_descriptor_type_sampled_image = 2,
  scegfx_descriptor_type_storage_image = 3,
  scegfx_descriptor_type_uniform_texel_buffer = 4,
  scegfx_descriptor_type_storage_texel_buffer = 5,
  scegfx_descriptor_type_uniform_buffer = 6,
  scegfx_descriptor_type_storage_buffer = 7,
  scegfx_descriptor_type_uniform_buffer_dynamic = 8,
  scegfx_descriptor_type_storage_buffer_dynamic = 9,
  scegfx_descriptor_type_input_attachment = 10,
} scegfx_descriptor_type_t;

typedef struct scegfx_descriptor_set_binding_t
{
  uint32_t binding;
  scegfx_descriptor_type_t descriptor_type;
  uint32_t descriptor_count;
  scegfx_stage_type_t stage;
  const void* immutable_samplers;
} scegfx_descriptor_set_binding_t;

typedef struct scegfx_descriptor_set_layout_create_info_t
{
  uint32_t binding_count;
  const scegfx_descriptor_set_binding_t* bindings;
} scegfx_descriptor_set_layout_create_info_t;

typedef struct scegfx_descriptor_set_layout_api_vtable_t
{
  bool (*initialize)(scegfx_descriptor_set_layout_t* this,
                     const scegfx_descriptor_set_layout_create_info_t* info);
  void (*terminate)(scegfx_descriptor_set_layout_t* this);
} scegfx_descriptor_set_layout_api_vtable_t;

typedef struct scegfx_descriptor_set_layout_t
{
  bool initialized;
  const scegfx_descriptor_set_layout_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_descriptor_set_layout_t;

#endif // SCEGFX_DESCRIPTOR_SET_LAYOUT_H

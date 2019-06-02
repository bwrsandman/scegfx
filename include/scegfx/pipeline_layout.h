//
// Created by sandy on 06/05/19.
//

#ifndef SCEGFX_PIPELINE_LAYOUT_H
#define SCEGFX_PIPELINE_LAYOUT_H

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

typedef struct scegfx_pipeline_layout_t scegfx_pipeline_layout_t;
typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_descriptor_set_layout_t scegfx_descriptor_set_layout_t;

typedef struct scegfx_push_constant_range_t
{
  scegfx_stage_type_t stage;
  uint32_t offset;
  uint32_t size;
} scegfx_push_constant_range_t;

typedef struct scegfx_pipeline_layout_create_info_t
{
  uint32_t set_layout_count;
  scegfx_descriptor_set_layout_t* const* set_layouts;
  uint32_t push_constant_range_count;
  const scegfx_push_constant_range_t* push_constant_ranges;
} scegfx_pipeline_layout_create_info_t;

typedef struct scegfx_pipeline_layout_api_vtable_t
{
  bool (*initialize)(scegfx_pipeline_layout_t* this,
                     const scegfx_pipeline_layout_create_info_t* info);
  void (*terminate)(scegfx_pipeline_layout_t* this);
} scegfx_pipeline_layout_api_vtable_t;

typedef struct scegfx_pipeline_layout_t
{
  bool initialized;
  const scegfx_pipeline_layout_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_pipeline_layout_t;

#endif // SCEGFX_PIPELINE_LAYOUT_H

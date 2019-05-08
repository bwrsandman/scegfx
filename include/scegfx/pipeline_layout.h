//
// Created by sandy on 06/05/19.
//

#ifndef SCEGFX_PIPELINE_LAYOUT_H
#define SCEGFX_PIPELINE_LAYOUT_H

#include <stdbool.h>

typedef struct scegfx_pipeline_layout_t scegfx_pipeline_layout_t;
typedef struct scegfx_context_t scegfx_context_t;

typedef struct scegfx_pipeline_layout_api_vtable_t
{
  bool (*initialize)(scegfx_pipeline_layout_t* this);
  void (*terminate)(scegfx_pipeline_layout_t* this);
} scegfx_pipeline_layout_api_vtable_t;

typedef struct scegfx_pipeline_layout_t
{
  bool initialized;
  const scegfx_pipeline_layout_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_pipeline_layout_t;

#endif // SCEGFX_PIPELINE_LAYOUT_H

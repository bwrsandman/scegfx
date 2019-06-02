//
// Created by sandy on 10/05/19.
//

#include "pipeline_layout_opengl.h"

#include <assert.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
bool
scegfx_pipeline_layout_opengl_initialize(
  scegfx_pipeline_layout_t* super,
  const scegfx_pipeline_layout_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_pipeline_layout_opengl_t* this =
    (scegfx_pipeline_layout_opengl_t*)super;

  this->super.initialized = true;
  return true;
}
#pragma clang diagnostic pop

void
scegfx_pipeline_layout_opengl_terminate(scegfx_pipeline_layout_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

//
// Created by sandy on 10/05/19.
//

#include "pipeline_layout_opengl.h"

#include <assert.h>

bool
scegfx_pipeline_layout_opengl_initialize(scegfx_pipeline_layout_t* super)
{
  assert(!super->initialized);
  scegfx_pipeline_layout_opengl_t* this =
    (scegfx_pipeline_layout_opengl_t*)super;

  this->super.initialized = true;
  return true;
}

void
scegfx_pipeline_layout_opengl_terminate(scegfx_pipeline_layout_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

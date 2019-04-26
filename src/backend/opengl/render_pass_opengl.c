//
// Created by sandy on 25/04/19.
//

#include "render_pass_opengl.h"

#include <assert.h>

bool
scegfx_render_pass_opengl_initialize(scegfx_render_pass_t* super)
{
  assert(!super->initialized);
  scegfx_render_pass_opengl_t* this = (scegfx_render_pass_opengl_t*)super;

  this->super.initialized = true;
  return true;
}

void
scegfx_render_pass_opengl_terminate(scegfx_render_pass_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

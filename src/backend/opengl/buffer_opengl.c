//
// Created by sandy on 09/05/19.
//

#include "buffer_opengl.h"

#include <assert.h>

bool
scegfx_buffer_opengl_initialize(scegfx_buffer_t* super,
                                scegfx_device_size_t size,
                                scegfx_buffer_usage_t usage)
{
  assert(!super->initialized);
  scegfx_buffer_opengl_t* this = (scegfx_buffer_opengl_t*)super;

  this->size = size;
  this->usage = usage;

  this->super.initialized = true;

  return true;
}

void
scegfx_buffer_opengl_terminate(scegfx_buffer_t* super)
{
  assert(super->initialized);
  scegfx_buffer_opengl_t* this = (scegfx_buffer_opengl_t*)super;

  this->handle = 0;

  super->initialized = false;
}

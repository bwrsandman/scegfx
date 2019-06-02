//
// Created by sandy on 09/06/19.
//

#include "descriptor_set_layout_opengl.h"

#include <assert.h>

#include "context_opengl.h"

bool
scegfx_descriptor_set_layout_opengl_initialize(
  scegfx_descriptor_set_layout_t* super,
  const scegfx_descriptor_set_layout_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_descriptor_set_layout_opengl_t* this =
    (scegfx_descriptor_set_layout_opengl_t*)super;

  assert(info->binding_count <= SCEGFX_OPENGL_MAX_BINDING_COUNT);

  this->binding_count = info->binding_count;
  for (uint32_t i = 0; i < info->binding_count; ++i) {
    this->bindings[i] = info->bindings[i].binding;
  }

  this->super.initialized = true;
  return false;
}

void
scegfx_descriptor_set_layout_opengl_terminate(
  scegfx_descriptor_set_layout_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

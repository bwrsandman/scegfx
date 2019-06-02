//
// Created by sandy on 09/06/19.
//

#ifndef SCEGFX_DESCRIPTOR_SET_LAYOUT_OPENGL_H
#define SCEGFX_DESCRIPTOR_SET_LAYOUT_OPENGL_H

#include <scegfx/descriptor_set_layout.h>

enum
{
  SCEGFX_OPENGL_MAX_BINDING_COUNT = 2,
};

typedef struct scegfx_descriptor_set_layout_opengl_t
{
  struct scegfx_descriptor_set_layout_t super;
  uint32_t binding_count;
  uint32_t bindings[SCEGFX_OPENGL_MAX_BINDING_COUNT];
} scegfx_descriptor_set_layout_opengl_t;

bool
scegfx_descriptor_set_layout_opengl_initialize(
  scegfx_descriptor_set_layout_t* this,
  const scegfx_descriptor_set_layout_create_info_t* info);
void
scegfx_descriptor_set_layout_opengl_terminate(
  scegfx_descriptor_set_layout_t* this);

const static scegfx_descriptor_set_layout_api_vtable_t
  scegfx_descriptor_set_layout_api_vtable_opengl = {
    .initialize = scegfx_descriptor_set_layout_opengl_initialize,
    .terminate = scegfx_descriptor_set_layout_opengl_terminate,
  };

#endif // SCEGFX_DESCRIPTOR_SET_LAYOUT_OPENGL_H

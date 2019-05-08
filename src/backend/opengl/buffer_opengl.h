//
// Created by sandy on 09/05/19.
//

#ifndef SCEGFX_BUFFER_OPENGL_H
#define SCEGFX_BUFFER_OPENGL_H

#include <scegfx/buffer.h>

typedef struct scegfx_buffer_opengl_t
{
  scegfx_buffer_t super;
  // Not managed directly by this object
  uint32_t handle;
  scegfx_device_size_t size;
  scegfx_buffer_usage_t usage;
} scegfx_buffer_opengl_t;

bool
scegfx_buffer_opengl_initialize(scegfx_buffer_t* this,
                                scegfx_device_size_t size,
                                scegfx_buffer_usage_t usage);
void
scegfx_buffer_opengl_terminate(scegfx_buffer_t* this);

const static scegfx_buffer_api_vtable_t scegfx_buffer_api_vtable_opengl = {
  .initialize = scegfx_buffer_opengl_initialize,
  .terminate = scegfx_buffer_opengl_terminate,
};

#endif // SCEGFX_BUFFER_OPENGL_H

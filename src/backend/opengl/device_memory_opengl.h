//
// Created by sandy on 09/05/19.
//

#ifndef SCEGFX_MEMORY_OPENGL_H
#define SCEGFX_MEMORY_OPENGL_H

#include <scegfx/device_memory.h>

typedef struct scegfx_device_memory_opengl_t
{
  scegfx_device_memory_t super;
  uint32_t handle;
  uint32_t bind_point;
#if defined(EMSCRIPTEN)
  void* stage;
  scegfx_device_size_t size;
#endif // EMSCRIPTEN
} scegfx_device_memory_opengl_t;

#endif // SCEGFX_MEMORY_OPENGL_H

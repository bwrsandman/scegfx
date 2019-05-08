//
// Created by sandy on 24/04/19.
//

#ifndef SCEGFX_SWAPCHAIN_OPENGL_H
#define SCEGFX_SWAPCHAIN_OPENGL_H

#include <scegfx/swapchain.h>

typedef struct scegfx_swapchain_opengl_t
{
  scegfx_swapchain_t super;
} scegfx_swapchain_opengl_t;

bool
scegfx_swapchain_opengl_initialize(scegfx_swapchain_t* this);
void
scegfx_swapchain_opengl_terminate(scegfx_swapchain_t* this);

const static scegfx_swapchain_api_vtable_t
  scegfx_swapchain_api_vtable_opengl = {
    .initialize = scegfx_swapchain_opengl_initialize,
    .terminate = scegfx_swapchain_opengl_terminate,
  };

#endif // SCEGFX_SWAPCHAIN_OPENGL_H

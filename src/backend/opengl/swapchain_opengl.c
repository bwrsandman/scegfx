//
// Created by sandy on 24/04/19.
//

#include "swapchain_opengl.h"

#include <assert.h>

#include "context_opengl.h"

bool
scegfx_swapchain_opengl_initialize(scegfx_swapchain_t* super)
{
  assert(!super->initialized);
  scegfx_swapchain_opengl_t* this = (scegfx_swapchain_opengl_t*)super;
  const scegfx_context_opengl_t* context =
    (const scegfx_context_opengl_t*)super->context;
  int width, height;

  this->super.flip_y = true;
  SDL_GL_GetDrawableSize(context->super.window_handle, &width, &height);
  this->super.extent.width = width;
  this->super.extent.height = height;

  this->super.initialized = true;
  return true;
}

void
scegfx_swapchain_opengl_terminate(scegfx_swapchain_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

//
// Created by sandy on 24/04/19.
//

#include "swapchain_opengl.h"

#include <assert.h>

#include "context_opengl.h"
#include "image_opengl.h"

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

  scegfx_image_opengl_t* image = &this->image;
  image->is_default_framebuffer = true;
  image->super.type = scegfx_image_type_2d;
  image->super.format = scegfx_format_r32g32b32a32_sfloat;
  image->format = /*GL_RGBA32F*/ 0x8814;
  image->super.extent.width = this->super.extent.width;
  image->super.extent.height = this->super.extent.height;
  image->super.extent.depth = 1;
  image->super.initialized = true;

  this->super.image_count = 1;

  this->super.initialized = true;
  return true;
}

void
scegfx_swapchain_opengl_terminate(scegfx_swapchain_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

scegfx_image_t*
scegfx_swapchain_opengl_get_image(scegfx_swapchain_t* super, uint32_t index)
{
  assert(super->initialized);
  assert(index == 0);

  scegfx_swapchain_opengl_t* this = (scegfx_swapchain_opengl_t*)super;

  return (scegfx_image_t*)&this->image;
}

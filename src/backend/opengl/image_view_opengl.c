//
// Created by sandy on 09/06/19.
//

#include "image_view_opengl.h"

#include "context_opengl.h"
#include "image_opengl.h"

#include <assert.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
bool
scegfx_image_view_opengl_initialize(scegfx_image_view_t* super,
                                    const scegfx_image_t* image,
                                    scegfx_image_aspect_t aspect)
{
#pragma clang diagnostic pop
  assert(!super->initialized);
  scegfx_image_view_opengl_t* this = (scegfx_image_view_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;
  const scegfx_image_opengl_t* image_gl = (const scegfx_image_opengl_t*)image;

  if (!image_gl->super.initialized) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Cannot create image view from uninitialized image");
    return false;
  }

  this->is_default_framebuffer = image_gl->is_default_framebuffer;
  this->image_type = image_gl->super.type;
  this->image_handle = image_gl->handle;

  this->super.initialized = true;
  return true;
}

void
scegfx_image_view_opengl_terminate(scegfx_image_view_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

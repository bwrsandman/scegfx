//
// Created by sandy on 09/06/19.
//

#ifndef SCEGFX_IMAGE_VIEW_OPENGL_H
#define SCEGFX_IMAGE_VIEW_OPENGL_H

#include <scegfx/image_view.h>

typedef struct scegfx_image_view_opengl_t
{
  scegfx_image_view_t super;
  bool is_default_framebuffer;
  uint32_t image_type;
  uint32_t image_handle;
} scegfx_image_view_opengl_t;

bool
scegfx_image_view_opengl_initialize(scegfx_image_view_t* this,
                                    const scegfx_image_t* image,
                                    scegfx_image_aspect_t aspect);
void
scegfx_image_view_opengl_terminate(scegfx_image_view_t* this);

const static scegfx_image_view_api_vtable_t
  scegfx_image_view_api_vtable_opengl = {
    .initialize = scegfx_image_view_opengl_initialize,
    .terminate = scegfx_image_view_opengl_terminate,
  };

#endif // SCEGFX_IMAGE_VIEW_OPENGL_H

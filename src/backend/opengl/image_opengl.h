//
// Created by sandy on 25/04/19.
//

#ifndef SCEGFX_IMAGE_OPENGL_H
#define SCEGFX_IMAGE_OPENGL_H

#include <scegfx/image.h>

typedef struct scegfx_image_opengl_t
{
  scegfx_image_t super;
  bool is_default_framebuffer;
  uint32_t handle;
  uint32_t target;
  uint32_t format;
  uint32_t type;
} scegfx_image_opengl_t;

bool
scegfx_image_opengl_initialize(scegfx_image_t* this,
                               const scegfx_image_create_info_t* info);
void
scegfx_image_opengl_terminate(scegfx_image_t* this);

const static scegfx_image_api_vtable_t scegfx_image_api_vtable_opengl = {
  .initialize = scegfx_image_opengl_initialize,
  .terminate = scegfx_image_opengl_terminate,
};

#endif // SCEGFX_IMAGE_OPENGL_H

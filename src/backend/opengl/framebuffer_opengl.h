//
// Created by sandy on 27/04/19.
//

#ifndef SCEGFX_FRAMEBUFFER_OPENGL_H
#define SCEGFX_FRAMEBUFFER_OPENGL_H

#include <scegfx/framebuffer.h>

#include <stdint.h>

enum
{
  SCEGFX_OPENGL_MAX_FRAMEBUFFER_ATTACHMENT_COUNT = 2,
};

typedef struct scegfx_framebuffer_opengl_t
{
  scegfx_framebuffer_t super;
  uint32_t handle;
  const scegfx_render_pass_t* render_pass;
} scegfx_framebuffer_opengl_t;

bool
scegfx_framebuffer_opengl_initialize(
  scegfx_framebuffer_t* this,
  const scegfx_framebuffer_create_info_t* info);
void
scegfx_framebuffer_opengl_terminate(scegfx_framebuffer_t* this);

const static scegfx_framebuffer_api_vtable_t
  scegfx_framebuffer_api_vtable_opengl = {
    .initialize = scegfx_framebuffer_opengl_initialize,
    .terminate = scegfx_framebuffer_opengl_terminate,
  };

#endif // SCEGFX_FRAMEBUFFER_OPENGL_H

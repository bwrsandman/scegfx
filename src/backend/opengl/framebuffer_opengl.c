//
// Created by sandy on 27/04/19.
//

#include "framebuffer_opengl.h"

#include <assert.h>

#include <SDL_opengl.h>

#include "image_view_opengl.h"

bool
scegfx_framebuffer_opengl_initialize(
  scegfx_framebuffer_t* super,
  const scegfx_framebuffer_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_framebuffer_opengl_t* this = (scegfx_framebuffer_opengl_t*)super;

  assert(info->attachment_count <=
         SCEGFX_OPENGL_MAX_FRAMEBUFFER_ATTACHMENT_COUNT);

  bool is_default_framebuffer = false;

  for (uint32_t i = 0; i < info->attachment_count; ++i) {
    const scegfx_image_view_opengl_t* view =
      (const scegfx_image_view_opengl_t*)info->attachments[i];
    if (view->is_default_framebuffer) {
      is_default_framebuffer = true;
    }
  }

  if (is_default_framebuffer) {
    this->handle = 0;
  } else {
    glGenFramebuffers(1, &this->handle);
    for (uint32_t i = 0; i < info->attachment_count; ++i) {
      //      glBindFramebuffer(GL_FRAMEBUFFER)
      const scegfx_image_view_opengl_t* view =
        (const scegfx_image_view_opengl_t*)info->attachments[i];
      assert(view);
      assert(false);
    }
  }

  this->render_pass = info->render_pass;

  super->initialized = true;

  return true;
}

void
scegfx_framebuffer_opengl_terminate(scegfx_framebuffer_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

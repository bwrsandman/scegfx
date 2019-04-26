//
// Created by sandy on 25/04/19.
//

#ifndef SCEGFX_RENDER_PASS_OPENGL_H
#define SCEGFX_RENDER_PASS_OPENGL_H

#include <scegfx/render_pass.h>

typedef struct scegfx_render_pass_opengl_t
{
  scegfx_render_pass_t super;
} scegfx_render_pass_opengl_t;

bool
scegfx_render_pass_opengl_initialize(scegfx_render_pass_t* this);
void
scegfx_render_pass_opengl_terminate(scegfx_render_pass_t* this);

const static scegfx_render_pass_api_vtable_t
  scegfx_render_pass_api_vtable_opengl = {
    .initialize = scegfx_render_pass_opengl_initialize,
    .terminate = scegfx_render_pass_opengl_terminate,
  };

#endif // SCEGFX_RENDER_PASS_OPENGL_H

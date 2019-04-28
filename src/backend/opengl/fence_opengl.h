//
// Created by sandy on 28/04/19.
//

#ifndef SCEGFX_FENCE_OPENGL_H
#define SCEGFX_FENCE_OPENGL_H

#include <scegfx/fence.h>

typedef struct __GLsync* GLsync;

typedef struct scegfx_fence_opengl_t
{
  scegfx_fence_t super;
  GLsync handle;
} scegfx_fence_opengl_t;

bool
scegfx_fence_opengl_initialize(scegfx_fence_t* this, bool signaled);
void
scegfx_fence_opengl_terminate(scegfx_fence_t* this);

bool
scegfx_fence_opengl_wait(scegfx_fence_t* this, uint64_t timeout);
bool
scegfx_fence_opengl_reset(scegfx_fence_t* this);

const static scegfx_fence_api_vtable_t scegfx_fence_api_vtable_opengl = {
  .initialize = scegfx_fence_opengl_initialize,
  .terminate = scegfx_fence_opengl_terminate,
  .wait = scegfx_fence_opengl_wait,
  .reset = scegfx_fence_opengl_reset,
};

bool
scegfx_fence_opengl_signal(scegfx_fence_opengl_t* this);

#endif // SCEGFX_FENCE_OPENGL_H

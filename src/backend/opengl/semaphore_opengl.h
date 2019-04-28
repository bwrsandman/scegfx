//
// Created by sandy on 28/04/19.
//

#ifndef SCEGFX_SEMAPHORE_OPENGL_H
#define SCEGFX_SEMAPHORE_OPENGL_H

#include <scegfx/semaphore.h>

typedef struct __GLsync* GLsync;

typedef struct scegfx_semaphore_opengl_t
{
  scegfx_semaphore_t super;
  GLsync handle;
} scegfx_semaphore_opengl_t;

bool
scegfx_semaphore_opengl_initialize(scegfx_semaphore_t* this);
void
scegfx_semaphore_opengl_terminate(scegfx_semaphore_t* this);

const static scegfx_semaphore_api_vtable_t
  scegfx_semaphore_api_vtable_opengl = {
    .initialize = scegfx_semaphore_opengl_initialize,
    .terminate = scegfx_semaphore_opengl_terminate,
  };

bool
scegfx_semaphore_opengl_signal(scegfx_semaphore_opengl_t* this);
bool
scegfx_semaphore_opengl_wait(scegfx_semaphore_opengl_t* this);

#endif // SCEGFX_SEMAPHORE_OPENGL_H

//
// Created by sandy on 28/04/19.
//

#include "semaphore_opengl.h"

#include <assert.h>

#include <SDL_opengl.h>

#include "context_opengl.h"

bool
scegfx_semaphore_opengl_initialize(scegfx_semaphore_t* super)
{
  assert(!super->initialized);
  scegfx_semaphore_opengl_t* this = (scegfx_semaphore_opengl_t*)super;

  this->super.initialized = true;
  return true;
}

void
scegfx_semaphore_opengl_terminate(scegfx_semaphore_t* super)
{
  assert(super->initialized);
  scegfx_semaphore_opengl_t* this = (scegfx_semaphore_opengl_t*)super;
  if (this->handle) {
    glDeleteSync(this->handle);
  }
  super->initialized = false;
}
bool
scegfx_semaphore_opengl_signal(scegfx_semaphore_opengl_t* this)
{
  assert(this->super.initialized);
  scegfx_context_opengl_t* context =
    (scegfx_context_opengl_t*)this->super.context;

  if (this->handle) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to signal new semaphore: already one present");
    return false;
  }

  this->handle = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

  if (this->handle == 0) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to signal new semaphore: unable to create sync object");
    return false;
  }

  return true;
}

bool
scegfx_semaphore_opengl_wait(scegfx_semaphore_opengl_t* this)
{
  assert(this->super.initialized);
  scegfx_context_opengl_t* context =
    (scegfx_context_opengl_t*)this->super.context;

  if (!this->handle) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to wait on semaphore: no fence present");
    return false;
  }

  glWaitSync(this->handle, 0, GL_TIMEOUT_IGNORED);

  glDeleteSync(this->handle);
  this->handle = NULL;

  return true;
}

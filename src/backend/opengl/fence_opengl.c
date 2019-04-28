//
// Created by sandy on 28/04/19.
//

#include "fence_opengl.h"

#include <assert.h>

#include <SDL_opengl.h>

#include "context_opengl.h"

bool
scegfx_fence_opengl_initialize(scegfx_fence_t* super, bool signaled)
{
  assert(!super->initialized);
  scegfx_fence_opengl_t* this = (scegfx_fence_opengl_t*)super;

  if (signaled) {
    this->handle = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_fence_opengl_terminate(scegfx_fence_t* super)
{
  assert(super->initialized);
  scegfx_fence_opengl_t* this = (scegfx_fence_opengl_t*)super;
  super->api_vtable->wait(super, this->super.max_wait_timeout);
  if (this->handle) {
    glDeleteSync(this->handle);
  }
  super->initialized = false;
}

bool
scegfx_fence_opengl_wait(scegfx_fence_t* super, uint64_t timeout)
{
  assert(super->initialized);
  scegfx_fence_opengl_t* this = (scegfx_fence_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;

  if (!this->handle) {
    return true;
  }

  if (timeout > this->super.max_wait_timeout) {
    context->super.debug_callback(
      scegfx_debug_severity_warning,
      __LINE__,
      FILE_BASENAME,
      "Specified timeout exceeds maximum fence timeout");
  }

  GLenum result = glClientWaitSync(this->handle, 0, timeout);

  switch (result) {
    case GL_ALREADY_SIGNALED:
    case GL_CONDITION_SATISFIED:
      return true;
    case GL_TIMEOUT_EXPIRED:
      context->super.debug_callback(scegfx_debug_severity_warning,
                                    __LINE__,
                                    FILE_BASENAME,
                                    "Timeout waiting for fence");
      return false;
    case GL_WAIT_FAILED: {
      GLenum err = glGetError();
      switch (err) {
        case GL_INVALID_OPERATION:
          context->super.debug_callback(
            scegfx_debug_severity_debug,
            __LINE__,
            FILE_BASENAME,
            "Wait failed on fence due to an invalid operation");
          break;
        default: {
          char buf[0x100];
          sprintf(buf, "Wait failed on fence due to OpenGL Error: 0x%04X", err);
          context->super.debug_callback(
            scegfx_debug_severity_debug, __LINE__, FILE_BASENAME, buf);
        } break;
      }
    }
      return false;
    default:
      context->super.debug_callback(scegfx_debug_severity_error,
                                    __LINE__,
                                    FILE_BASENAME,
                                    "Unable to wait for fence");
      assert(false);
      return false;
  }
}

bool
scegfx_fence_opengl_reset(scegfx_fence_t* super)
{
  assert(super->initialized);
  scegfx_fence_opengl_t* this = (scegfx_fence_opengl_t*)super;
  if (this->handle) {
    glDeleteSync(this->handle);
  }
  this->handle = NULL;
  return true;
}

bool
scegfx_fence_opengl_signal(scegfx_fence_opengl_t* this)
{
  assert(this->super.initialized);
  scegfx_context_opengl_t* context =
    (scegfx_context_opengl_t*)this->super.context;

  if (this->handle) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to signal new fence: already one present");
    return false;
  }

  this->handle = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

  if (this->handle == 0) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to signal new fence: unable to create sync object");
    return false;
  }

  return true;
}

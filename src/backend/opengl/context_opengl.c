//
// Created by sandy on 21/03/19.
//

#include "context_opengl.h"

#include <assert.h>
#include <threads.h>

#include <SDL_opengl.h>
#include <SDL_video.h>

#include "fence_opengl.h"
#include "semaphore_opengl.h"
#include "swapchain_opengl.h"

#define GL_MAX_CLIENT_WAIT_TIMEOUT_WEBGL 0x9247

#if defined(EMSCRIPTEN)
thread_local EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_handle = 0;
#else
thread_local SDL_GLContext sdl_handle = NULL;
#endif

#if !defined(NDEBUG) && !defined(EMSCRIPTEN)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void
scegfx_backend_opengl_debug_callback(GLenum source,
                                     GLenum type,
                                     GLuint msg_code,
                                     GLenum severity,
                                     GLsizei length,
                                     const char* msg,
                                     const void* user_data)
{
#pragma clang diagnostic pop
  if (user_data == NULL || !msg || msg[0] == 0)
    return;
  const scegfx_context_opengl_t* this = user_data;
  scegfx_debug_severity_t level = scegfx_debug_severity_info;
  if (type == GL_DEBUG_TYPE_ERROR) {
    level = scegfx_debug_severity_error;
  } else if (severity == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) {
    level = scegfx_debug_severity_warning;
  } else if (severity == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
    level = scegfx_debug_severity_warning;
  } else if (severity == GL_DEBUG_TYPE_PORTABILITY) {
    level = scegfx_debug_severity_warning;
  } else if (severity == GL_DEBUG_TYPE_PERFORMANCE) {
    level = scegfx_debug_severity_performance_warning;
  } else if (severity == GL_DEBUG_TYPE_OTHER) {
    level = scegfx_debug_severity_info;
  } else if (severity == GL_DEBUG_TYPE_MARKER) {
    level = scegfx_debug_severity_debug;
  } else if (severity == GL_DEBUG_TYPE_PUSH_GROUP) {
    level = scegfx_debug_severity_debug;
  } else if (severity == GL_DEBUG_TYPE_POP_GROUP) {
    level = scegfx_debug_severity_debug;
  } else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
    level = scegfx_debug_severity_debug;
  }
  /*const char* severity_string = "Unknown";
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
    severity_string = "Notification";
  } else if (severity == GL_DEBUG_SEVERITY_LOW) {
    severity_string = "Low";
  } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
    severity_string = "Medium";
  } else if (severity == GL_DEBUG_SEVERITY_HIGH) {
    severity_string = "High";
  }*/
  const char* source_string = "Unknown";
  if (source == GL_DEBUG_SOURCE_API) {
    source_string = "API";
  } else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM) {
    source_string = "Windowing system";
  } else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) {
    source_string = "Shader Compiler";
  } else if (source == GL_DEBUG_SOURCE_THIRD_PARTY) {
    source_string = "Third Party";
  } else if (source == GL_DEBUG_SOURCE_APPLICATION) {
    source_string = "Application";
  } else if (source == GL_DEBUG_SOURCE_OTHER) {
    source_string = "Other";
  }

  this->super.debug_callback(
    level, msg_code, source_string, /*severity_string,*/ msg);
}
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
bool
scegfx_context_opengl_initialize(scegfx_context_t* super)
{
#pragma clang diagnostic pop
  assert(!super->initialized);
  scegfx_context_opengl_t* this = (scegfx_context_opengl_t*)super;

  char message[0x100];
#if defined(EMSCRIPTEN)
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.antialias = true;
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;
  attrs.alpha = false;
  webgl_handle = emscripten_webgl_create_context(NULL, &attrs);
  if (webgl_handle <= 0) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to create webgl context");
    return false;
  }
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(webgl_handle);
  if (res != EMSCRIPTEN_RESULT_SUCCESS) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to make context current");
    emscripten_webgl_destroy_context(webgl_handle);
    webgl_handle = 0;
    return false;
  }
#else
  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  sdl_handle = SDL_GL_CreateContext(this->super.window_handle);
  if (sdl_handle == NULL) {
    this->super.debug_callback(
      scegfx_debug_severity_error, __LINE__, FILE_BASENAME, SDL_GetError());
    return false;
  }
#endif

#if !defined(NDEBUG) && !defined(EMSCRIPTEN)
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(scegfx_backend_opengl_debug_callback, this);
#endif

#if defined(EMSCRIPTEN)
  const char* opengl_version = (const char*)glGetString(GL_VERSION);
  sprintf(message, "OpenGL Version: %s", opengl_version);
#else
  GLint major_version = 0;
  GLint minor_version = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &major_version);
  glGetIntegerv(GL_MINOR_VERSION, &minor_version);
  sprintf(message, "OpenGL Version: %d.%d", major_version, minor_version);
#endif
  this->super.debug_callback(
    scegfx_debug_severity_info, __LINE__, FILE_BASENAME, message);

  const char* glsl_version =
    (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
  sprintf(message, "GLSL Version: %s", glsl_version);
  this->super.debug_callback(
    scegfx_debug_severity_info, __LINE__, FILE_BASENAME, message);

  // Zero out
  this->extension_count = 0;
  memset(this->extension_names, 0, sizeof(this->extension_names));
  // Get available extensions
  int32_t extension_count = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);
  assert(extension_count > 0);
  this->extension_count = (uint32_t)extension_count;
  this->super.debug_callback(scegfx_debug_severity_info,
                             __LINE__,
                             FILE_BASENAME,
                             "Available extensions:");
  for (uint32_t i = 0; i < this->extension_count; ++i) {
    const char* extension_name = (const char*)glGetStringi(GL_EXTENSIONS, i);
    strncpy(this->extension_names[i],
            extension_name,
            SCEGFX_OPENGL_MAX_EXTENSION_STRING_SIZE * sizeof(char));
    this->super.debug_callback(scegfx_debug_severity_info,
                               __LINE__,
                               FILE_BASENAME,
                               this->extension_names[i]);
  }

  this->max_client_wait_timeout = UINT64_MAX;
#if defined(EMSCRIPTEN)
  glGetInteger64v(GL_MAX_CLIENT_WAIT_TIMEOUT_WEBGL,
                  (int64_t*)&this->max_client_wait_timeout);
#endif // defined(EMSCRIPTEN)
  this->max_server_wait_timeout = 0;
  glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT,
                  (int64_t*)&this->max_server_wait_timeout);

  SDL_GL_SetSwapInterval(0);

  super->initialized = true;
  return true;
}

void
scegfx_context_opengl_terminate(scegfx_context_t* super)
{
  assert(super->initialized);
#if defined(EMSCRIPTEN)
  emscripten_webgl_destroy_context(webgl_handle);
#else
  SDL_GL_DeleteContext(sdl_handle);
#endif
  super->initialized = false;
}

scegfx_fence_t*
scegfx_context_opengl_create_fence(scegfx_context_t* super,
                                   scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_context_opengl_t* this = (scegfx_context_opengl_t*)super;
  scegfx_fence_t* fence = NULL;
  if (allocator == NULL)
    fence = malloc(sizeof(scegfx_fence_opengl_t));
  else
    fence = allocator->allocator_callback(
      NULL, sizeof(scegfx_fence_opengl_t), allocator->user_data);
  memset(fence, 0, sizeof(scegfx_fence_opengl_t));

  fence->api_vtable = &scegfx_fence_api_vtable_opengl;
  fence->context = super;
  uint64_t* mutable_max_wait_timeout = (uint64_t*)&fence->max_wait_timeout;
  *mutable_max_wait_timeout = this->max_client_wait_timeout;

  return fence;
}

void
scegfx_context_opengl_destroy_fence(scegfx_context_t* this,
                                    scegfx_fence_t* fence,
                                    scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(fence);
  } else {
    allocator->allocator_callback(fence, 0, allocator->user_data);
  }
}

scegfx_semaphore_t*
scegfx_context_opengl_create_semaphore(scegfx_context_t* super,
                                       scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_context_opengl_t* this = (scegfx_context_opengl_t*)super;
  scegfx_semaphore_t* semaphore = NULL;
  if (allocator == NULL)
    semaphore = malloc(sizeof(scegfx_semaphore_opengl_t));
  else
    semaphore = allocator->allocator_callback(
      NULL, sizeof(scegfx_semaphore_opengl_t), allocator->user_data);
  memset(semaphore, 0, sizeof(scegfx_semaphore_opengl_t));

  semaphore->api_vtable = &scegfx_semaphore_api_vtable_opengl;
  semaphore->context = super;
  uint64_t* mutable_max_wait_timeout = (uint64_t*)&semaphore->max_wait_timeout;
  *mutable_max_wait_timeout = this->max_server_wait_timeout;

  return semaphore;
}

void
scegfx_context_opengl_destroy_semaphore(scegfx_context_t* this,
                                        scegfx_semaphore_t* semaphore,
                                        scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(semaphore);
  } else {
    allocator->allocator_callback(semaphore, 0, allocator->user_data);
  }
}

scegfx_swapchain_t*
scegfx_context_opengl_create_swapchain(scegfx_context_t* this,
                                       scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  scegfx_swapchain_t* swapchain = NULL;
  if (allocator == NULL)
    swapchain = malloc(sizeof(scegfx_swapchain_opengl_t));
  else
    swapchain = allocator->allocator_callback(
      NULL, sizeof(scegfx_swapchain_opengl_t), allocator->user_data);
  memset(swapchain, 0, sizeof(scegfx_swapchain_opengl_t));

  swapchain->api_vtable = &scegfx_swapchain_api_vtable_gles;
  swapchain->context = this;

  return swapchain;
}

void
scegfx_context_opengl_destroy_swapchain(scegfx_context_t* this,
                                        scegfx_swapchain_t* swapchain,
                                        scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(swapchain);
  } else {
    allocator->allocator_callback(swapchain, 0, allocator->user_data);
  }
}

bool
scegfx_context_opengl_make_current(scegfx_context_t* super)
{
  assert(super->initialized);
  scegfx_context_opengl_t* this = (scegfx_context_opengl_t*)super;
#if defined(EMSCRIPTEN)
  if (webgl_handle == 0) {
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.antialias = true;
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.alpha = false;
    webgl_handle = emscripten_webgl_create_context(NULL, &attrs);
    if (webgl_handle <= 0) {
      this->super.debug_callback(scegfx_debug_severity_error,
                                 __LINE__,
                                 FILE_BASENAME,
                                 "Unable to create webgl context");
      return false;
    }
  }
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(webgl_handle);
  if (res != EMSCRIPTEN_RESULT_SUCCESS) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to make context current");
    emscripten_webgl_destroy_context(webgl_handle);
    webgl_handle = 0;
    return false;
  }
#else
  if (sdl_handle == NULL) {
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    sdl_handle = SDL_GL_CreateContext(this->super.window_handle);
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(scegfx_backend_opengl_debug_callback, this);

  int res = SDL_GL_MakeCurrent(this->super.window_handle, sdl_handle);
  if (res != 0) {
    this->super.debug_callback(
      scegfx_debug_severity_error, __LINE__, FILE_BASENAME, SDL_GetError());
    return false;
  }
#endif
  return true;
}
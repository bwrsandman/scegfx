//
// Created by sandy on 21/03/19.
//

#ifndef SCEGFX_CONTEXT_OPENGL_H
#define SCEGFX_CONTEXT_OPENGL_H

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL_video.h>

#include <scegfx/context.h>

enum
{
  SCEGFX_OPENGL_MAX_EXTENSION_STRING_SIZE = 64
};
enum
{
  SCEGFX_OPENGL_MAX_EXTENSION_COUNT = 512
};

typedef struct scegfx_context_opengl_t
{
  scegfx_context_t super;
  uint32_t extension_count;
  char extension_names[SCEGFX_OPENGL_MAX_EXTENSION_COUNT]
                      [SCEGFX_OPENGL_MAX_EXTENSION_STRING_SIZE];
  uint64_t max_client_wait_timeout;
  uint64_t max_server_wait_timeout;
} scegfx_context_opengl_t;

bool
scegfx_context_opengl_initialize(scegfx_context_t* this);
void
scegfx_context_opengl_terminate(scegfx_context_t* this);

scegfx_swapchain_t*
scegfx_context_opengl_create_swapchain(scegfx_context_t* this,
                                       scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_swapchain(scegfx_context_t* this,
                                        scegfx_swapchain_t* swapchain,
                                        scegfx_allocator_t* allocator);

scegfx_fence_t*
scegfx_context_opengl_create_fence(scegfx_context_t* this,
                                   scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_fence(scegfx_context_t* this,
                                    scegfx_fence_t* fence,
                                    scegfx_allocator_t* allocator);

scegfx_semaphore_t*
scegfx_context_opengl_create_semaphore(scegfx_context_t* this,
                                       scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_semaphore(scegfx_context_t* this,
                                        scegfx_semaphore_t* semaphore,
                                        scegfx_allocator_t* allocator);

bool
scegfx_context_opengl_make_current(scegfx_context_t* this);

static const scegfx_context_api_vtable_t scegfx_context_api_vtable_opengl = {
  .initialize = scegfx_context_opengl_initialize,
  .terminate = scegfx_context_opengl_terminate,
  .create_swapchain = scegfx_context_opengl_create_swapchain,
  .destroy_swapchain = scegfx_context_opengl_destroy_swapchain,
  .create_fence = scegfx_context_opengl_create_fence,
  .destroy_fence = scegfx_context_opengl_destroy_fence,
  .create_semaphore = scegfx_context_opengl_create_semaphore,
  .destroy_semaphore = scegfx_context_opengl_destroy_semaphore,
  .make_current = scegfx_context_opengl_make_current,
};

#endif // SCEGFX_CONTEXT_OPENGL_H
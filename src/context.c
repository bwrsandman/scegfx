//
// Created by sandy on 23/03/19.
//

#include <scegfx/context.h>

#include <assert.h>

#include <scegfx/allocator.h>

#if SCEGFX_SUPPORTS_VULKAN
#include "backend/vulkan/context_vulkan.h"
#endif
#if SCEGFX_SUPPORTS_WEBGPU
#include "backend/webgpu/context_webgpu.h"
#endif
#include "backend/opengl/context_opengl.h"

scegfx_context_t*
scegfx_context_create(scegfx_backend_api_t backend,
                      scegfx_allocator_t* allocator,
                      SDL_Window* window,
                      scegfx_debug_callback_t debug_callback)
{
  const scegfx_context_api_vtable_t* api_vtable;
  scegfx_context_t* context = NULL;
  switch (backend) {
#if SCEGFX_SUPPORTS_VULKAN
    case scegfx_backend_api_vulkan:
      api_vtable = &scegfx_context_api_vtable_vulkan;
      if (allocator == NULL)
        context = malloc(sizeof(scegfx_context_vulkan_t));
      else
        context = allocator->allocator_callback(
          NULL, sizeof(scegfx_context_vulkan_t), allocator->user_data);
      memset(context, 0, sizeof(scegfx_context_vulkan_t));
      break;
#endif
#if SCEGFX_SUPPORTS_WEBGPU
    case scegfx_backend_api_webgpu:
      api_vtable = &scegfx_context_api_vtable_webgpu;
      if (allocator == NULL)
        context = malloc(sizeof(scegfx_context_webgpu_t));
      else
        context = allocator->allocator_callback(
          NULL, sizeof(scegfx_context_webgpu_t), allocator->user_data);
      memset(context, 0, sizeof(scegfx_context_webgpu_t));
      break;
#endif
    case scegfx_backend_api_opengl:
      api_vtable = &scegfx_context_api_vtable_opengl;
      if (allocator == NULL)
        context = malloc(sizeof(scegfx_context_opengl_t));
      else
        context = allocator->allocator_callback(
          NULL, sizeof(scegfx_context_opengl_t), allocator->user_data);
      memset(context, 0, sizeof(scegfx_context_opengl_t));
      break;
    default:
      return NULL;
  }

  context->api_vtable = api_vtable;
  context->window_handle = window;
  context->debug_callback = debug_callback;

  return context;
}

void
scegfx_context_destroy(scegfx_context_t* this, scegfx_allocator_t* allocator)
{
  assert(!this->initialized);
  if (allocator == NULL) {
    free(this);
  } else {
    allocator->allocator_callback(this, 0, allocator->user_data);
  }
}

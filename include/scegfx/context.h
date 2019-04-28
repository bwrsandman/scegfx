#ifndef SCEGFX_CONTEXT_H
#define SCEGFX_CONTEXT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct SDL_Window SDL_Window;

typedef struct scegfx_private_window_t scegfx_private_window_t;
typedef struct scegfx_allocator_t scegfx_allocator_t;
typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_fence_t scegfx_fence_t;
typedef struct scegfx_swapchain_t scegfx_swapchain_t;

typedef enum scegfx_backend_api_t
{
  scegfx_backend_api_vulkan,
  scegfx_backend_api_webgpu,
  scegfx_backend_api_opengl,
} scegfx_backend_api_t;

typedef enum scegfx_debug_severity_t
{
  scegfx_debug_severity_debug,
  scegfx_debug_severity_info,
  scegfx_debug_severity_warning,
  scegfx_debug_severity_performance_warning,
  scegfx_debug_severity_error,
} scegfx_debug_severity_t;

typedef struct scegfx_context_api_vtable_t
{
  bool (*initialize)(scegfx_context_t* this);
  void (*terminate)(scegfx_context_t* this);

  scegfx_fence_t* (*create_fence)(scegfx_context_t* this,
                                  scegfx_allocator_t* allocator);
  void (*destroy_fence)(scegfx_context_t* this,
                        scegfx_fence_t* fence,
                        scegfx_allocator_t* allocator);

  scegfx_swapchain_t* (*create_swapchain)(scegfx_context_t* this,
                                          scegfx_allocator_t* allocator);
  void (*destroy_swapchain)(scegfx_context_t* this,
                            scegfx_swapchain_t* swapchain,
                            scegfx_allocator_t* allocator);

  bool (*make_current)(scegfx_context_t* this);
} scegfx_context_api_vtable_t;

typedef void (*scegfx_debug_callback_t)(scegfx_debug_severity_t level,
                                        uint32_t msg_code,
                                        const char* source,
                                        const char* message);

typedef struct scegfx_context_t
{
  bool initialized;
  const scegfx_context_api_vtable_t* api_vtable;
  SDL_Window* window_handle;
  scegfx_debug_callback_t debug_callback;
} scegfx_context_t;

scegfx_context_t*
scegfx_context_create(scegfx_backend_api_t backend,
                      scegfx_allocator_t* allocator,
                      SDL_Window* window,
                      scegfx_debug_callback_t debug_callback);
void
scegfx_context_destroy(scegfx_context_t* this, scegfx_allocator_t* allocator);

#endif // SCEGFX_CONTEXT_H

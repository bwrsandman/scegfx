#ifndef SCEGFX_CONTEXT_H
#define SCEGFX_CONTEXT_H

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

typedef struct SDL_Window SDL_Window;

typedef struct scegfx_allocator_t scegfx_allocator_t;
typedef struct scegfx_buffer_t scegfx_buffer_t;
typedef struct scegfx_command_buffer_t scegfx_command_buffer_t;
typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_device_memory_t scegfx_device_memory_t;
typedef struct scegfx_device_memory_allocate_info_t
  scegfx_device_memory_allocate_info_t;
typedef struct scegfx_device_memory_requirements_t
  scegfx_device_memory_requirements_t;
typedef struct scegfx_framebuffer_t scegfx_framebuffer_t;
typedef struct scegfx_image_t scegfx_image_t;
typedef struct scegfx_image_view_t scegfx_image_view_t;
typedef struct scegfx_mapped_device_memory_range_t
  scegfx_mapped_device_memory_range_t;
typedef struct scegfx_fence_t scegfx_fence_t;
typedef struct scegfx_render_pass_t scegfx_render_pass_t;
typedef struct scegfx_sampler_t scegfx_sampler_t;
typedef struct scegfx_semaphore_t scegfx_semaphore_t;
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

  scegfx_semaphore_t* (*create_semaphore)(scegfx_context_t* this,
                                          scegfx_allocator_t* allocator);
  void (*destroy_semaphore)(scegfx_context_t* this,
                            scegfx_semaphore_t* semaphore,
                            scegfx_allocator_t* allocator);

  uint32_t (*get_memory_type)(scegfx_context_t* this,
                              uint32_t type_bits,
                              scegfx_memory_properties_t properties);
  scegfx_device_memory_t* (*allocate_memory)(
    scegfx_context_t* this,
    const scegfx_device_memory_allocate_info_t* info,
    scegfx_allocator_t* allocator);
  void (*free_memory)(scegfx_context_t* this,
                      scegfx_device_memory_t* memory,
                      scegfx_allocator_t* allocator);
  bool (*map_memory)(scegfx_context_t* this,
                     scegfx_device_memory_t* memory,
                     scegfx_device_size_t offset,
                     scegfx_device_size_t size,
                     void** data);
  void (*unmap_memory)(scegfx_context_t* this, scegfx_device_memory_t* memory);

  bool (*flush_mapped_memory_ranges)(
    scegfx_context_t* this,
    uint32_t memory_range_count,
    const scegfx_mapped_device_memory_range_t* memory_ranges);

  scegfx_buffer_t* (*create_buffer)(scegfx_context_t* this,
                                    scegfx_allocator_t* allocator);
  void (*destroy_buffer)(scegfx_context_t* this,
                         scegfx_buffer_t* buffer,
                         scegfx_allocator_t* allocator);
  void (*get_buffer_memory_requirements)(
    scegfx_context_t* this,
    const scegfx_buffer_t* buffer,
    scegfx_device_memory_requirements_t* memory_requirements);

  bool (*bind_buffer_memory)(scegfx_context_t* this,
                             scegfx_buffer_t* buffer,
                             scegfx_device_memory_t* memory,
                             scegfx_device_size_t memory_offset);
  bool (*bind_image_memory)(scegfx_context_t* this,
                            scegfx_image_t* image,
                            scegfx_device_memory_t* memory,
                            scegfx_device_size_t memory_offset);

  scegfx_swapchain_t* (*create_swapchain)(scegfx_context_t* this,
                                          scegfx_allocator_t* allocator);
  void (*destroy_swapchain)(scegfx_context_t* this,
                            scegfx_swapchain_t* swapchain,
                            scegfx_allocator_t* allocator);

  scegfx_image_t* (*create_image)(scegfx_context_t* this,
                                  scegfx_allocator_t* allocator);
  void (*destroy_image)(scegfx_context_t* this,
                        scegfx_image_t* image,
                        scegfx_allocator_t* allocator);
  void (*get_image_memory_requirements)(
    const scegfx_context_t* this,
    const scegfx_image_t* image,
    scegfx_device_memory_requirements_t* memory_requirements);

  scegfx_image_view_t* (*create_image_view)(scegfx_context_t* this,
                                            scegfx_allocator_t* allocator);
  void (*destroy_image_view)(scegfx_context_t* this,
                             scegfx_image_view_t* view,
                             scegfx_allocator_t* allocator);

  scegfx_sampler_t* (*create_sampler)(scegfx_context_t* this,
                                      scegfx_allocator_t* allocator);
  void (*destroy_sampler)(scegfx_context_t* this,
                          scegfx_sampler_t* sampler,
                          scegfx_allocator_t* allocator);

  scegfx_render_pass_t* (*create_render_pass)(scegfx_context_t* this,
                                              scegfx_allocator_t* allocator);
  void (*destroy_render_pass)(scegfx_context_t* this,
                              scegfx_render_pass_t* render_pass,
                              scegfx_allocator_t* allocator);

  scegfx_framebuffer_t* (*create_framebuffer)(scegfx_context_t* this,
                                              scegfx_allocator_t* allocator);
  void (*destroy_framebuffer)(scegfx_context_t* this,
                              scegfx_framebuffer_t* framebuffer,
                              scegfx_allocator_t* allocator);

  scegfx_command_buffer_t* (*create_command_buffer)(
    scegfx_context_t* this,
    scegfx_allocator_t* allocator);
  void (*destroy_command_buffer)(scegfx_context_t* this,
                                 scegfx_command_buffer_t* command_buffer,
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

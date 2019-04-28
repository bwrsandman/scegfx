//
// Created by sandy on 21/03/19.
//

#ifndef SCEGFX_CONTEXT_VULKAN_H
#define SCEGFX_CONTEXT_VULKAN_H

#include <scegfx/context.h>

#include <vulkan/vulkan.h>

enum
{
  SCEGFX_VULKAN_MAX_EXTENSION_STRING_SIZE = 32,
  SCEGFX_VULKAN_MAX_EXTENSION_COUNT = 128,
  SCEGFX_VULKAN_MAX_LAYER_STRING_SIZE = 64,
  SCEGFX_VULKAN_MAX_LAYER_COUNT = 64,
  SCEGFX_VULKAN_MAX_SURFACE_PRESENT_MODES_COUNT = 32,
};

typedef struct scegfx_context_vulkan_t
{
  scegfx_context_t super;
  struct
  {
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR GetPhysicalDeviceSurfaceFormats;
  } functions;
  uint32_t instance_extension_count;
  char instance_extension_names[SCEGFX_VULKAN_MAX_EXTENSION_COUNT]
                               [SCEGFX_VULKAN_MAX_EXTENSION_STRING_SIZE];
  uint32_t instance_layer_count;
  char instance_layer_names[SCEGFX_VULKAN_MAX_LAYER_COUNT]
                           [SCEGFX_VULKAN_MAX_LAYER_STRING_SIZE];
  VkInstance instance;
  VkSurfaceKHR surface;
  VkSurfaceFormatKHR surface_format;
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkFormat depth_format;
  uint32_t surface_present_mode_count;
  VkPresentModeKHR
    surface_present_modes[SCEGFX_VULKAN_MAX_SURFACE_PRESENT_MODES_COUNT];
  uint32_t device_extension_count;
  char device_extension_names[SCEGFX_VULKAN_MAX_EXTENSION_COUNT]
                             [SCEGFX_VULKAN_MAX_EXTENSION_STRING_SIZE];
  uint32_t device_layer_count;
  char device_layer_names[SCEGFX_VULKAN_MAX_LAYER_COUNT]
                         [SCEGFX_VULKAN_MAX_LAYER_STRING_SIZE];
  VkPhysicalDeviceMemoryProperties device_memory_properties;
  VkDevice device;
  uint32_t present_graphics_compute_transfer_queue_index;
  VkQueue present_graphics_compute_transfer_queue;
  VkCommandPool command_pool;
} scegfx_context_vulkan_t;

bool
scegfx_context_vulkan_initialize(scegfx_context_t* this);
void
scegfx_context_vulkan_terminate(scegfx_context_t* this);

uint32_t
scegfx_context_vulkan_get_memory_type(scegfx_context_t* this,
                                      uint32_t type_bits,
                                      scegfx_memory_properties_t properties);
scegfx_device_memory_t*
scegfx_context_vulkan_allocate_memory(
  scegfx_context_t* this,
  const scegfx_device_memory_allocate_info_t* info,
  scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_free_memory(scegfx_context_t* this,
                                  scegfx_device_memory_t* memory,
                                  scegfx_allocator_t* allocator);
bool
scegfx_context_vulkan_map_memory(scegfx_context_t* this,
                                 scegfx_device_memory_t* memory,
                                 scegfx_device_size_t offset,
                                 scegfx_device_size_t size,
                                 void** data);
void
scegfx_context_vulkan_unmap_memory(scegfx_context_t* this,
                                   scegfx_device_memory_t* memory);

bool
scegfx_context_vulkan_flush_mapped_memory_ranges(
  scegfx_context_t* this,
  uint32_t memory_range_count,
  const scegfx_mapped_device_memory_range_t* memory_ranges);

scegfx_buffer_t*
scegfx_context_vulkan_create_buffer(scegfx_context_t* this,
                                    scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_destroy_buffer(scegfx_context_t* this,
                                     scegfx_buffer_t* buffer,
                                     scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_get_buffer_memory_requirements(
  scegfx_context_t* this,
  const scegfx_buffer_t* buffer,
  scegfx_device_memory_requirements_t* memory_requirements);

bool
scegfx_context_vulkan_bind_buffer_memory(scegfx_context_t* this,
                                         scegfx_buffer_t* buffer,
                                         scegfx_device_memory_t* memory,
                                         scegfx_device_size_t memory_offset);
bool
scegfx_context_vulkan_bind_image_memory(scegfx_context_t* this,
                                        scegfx_image_t* image,
                                        scegfx_device_memory_t* memory,
                                        scegfx_device_size_t memory_offset);

scegfx_swapchain_t*
scegfx_context_vulkan_create_swapchain(scegfx_context_t* this,
                                       scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_destroy_swapchain(scegfx_context_t* this,
                                        scegfx_swapchain_t* swapchain,
                                        scegfx_allocator_t* allocator);

scegfx_fence_t*
scegfx_context_vulkan_create_fence(scegfx_context_t* this,
                                   scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_destroy_fence(scegfx_context_t* this,
                                    scegfx_fence_t* fence,
                                    scegfx_allocator_t* allocator);

scegfx_semaphore_t*
scegfx_context_vulkan_create_semaphore(scegfx_context_t* this,
                                       scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_destroy_semaphore(scegfx_context_t* this,
                                        scegfx_semaphore_t* semaphore,
                                        scegfx_allocator_t* allocator);

scegfx_image_t*
scegfx_context_vulkan_create_image(scegfx_context_t* this,
                                   scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_destroy_image(scegfx_context_t* this,
                                    scegfx_image_t* image,
                                    scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_get_image_memory_requirements(
  const scegfx_context_t* this,
  const scegfx_image_t* image,
  scegfx_device_memory_requirements_t* memory_requirements);

scegfx_image_view_t*
scegfx_vulkan_create_image_view(scegfx_context_t* this,
                                scegfx_allocator_t* allocator);
void
scegfx_vulkan_destroy_image_view(scegfx_context_t* this,
                                 scegfx_image_view_t* view,
                                 scegfx_allocator_t* allocator);

scegfx_sampler_t*
scegfx_context_vulkan_create_sampler(scegfx_context_t* this,
                                     scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_destroy_sampler(scegfx_context_t* this,
                                      scegfx_sampler_t* sampler,
                                      scegfx_allocator_t* allocator);

scegfx_render_pass_t*
scegfx_vulkan_create_render_pass(scegfx_context_t* this,
                                 scegfx_allocator_t* allocator);
void
scegfx_vulkan_destroy_render_pass(scegfx_context_t* this,
                                  scegfx_render_pass_t* render_pass,
                                  scegfx_allocator_t* allocator);

scegfx_framebuffer_t*
scegfx_vulkan_create_framebuffer(scegfx_context_t* this,
                                 scegfx_allocator_t* allocator);
void
scegfx_vulkan_destroy_framebuffer(scegfx_context_t* this,
                                  scegfx_framebuffer_t* framebuffer,
                                  scegfx_allocator_t* allocator);

scegfx_command_buffer_t*
scegfx_context_vulkan_create_command_buffer(scegfx_context_t* super,
                                            scegfx_allocator_t* allocator);
void
scegfx_context_vulkan_destroy_command_buffer(scegfx_context_t* super,
                                             scegfx_command_buffer_t* queue,
                                             scegfx_allocator_t* allocator);

bool
scegfx_context_vulkan_make_current(scegfx_context_t* this);
bool
scegfx_context_vulkan_submit_to_queue(scegfx_context_t* this,
                                      scegfx_submit_info_t* info,
                                      scegfx_fence_t* fence);
bool
scegfx_context_vulkan_present(scegfx_context_t* this,
                              scegfx_present_info_t* info);
bool
scegfx_context_vulkan_wait_idle(scegfx_context_t* this);

static const scegfx_context_api_vtable_t scegfx_context_api_vtable_vulkan = {
  .initialize = scegfx_context_vulkan_initialize,
  .terminate = scegfx_context_vulkan_terminate,
  .get_memory_type = scegfx_context_vulkan_get_memory_type,
  .allocate_memory = scegfx_context_vulkan_allocate_memory,
  .free_memory = scegfx_context_vulkan_free_memory,
  .map_memory = scegfx_context_vulkan_map_memory,
  .unmap_memory = scegfx_context_vulkan_unmap_memory,
  .flush_mapped_memory_ranges =
    scegfx_context_vulkan_flush_mapped_memory_ranges,
  .create_buffer = scegfx_context_vulkan_create_buffer,
  .destroy_buffer = scegfx_context_vulkan_destroy_buffer,
  .get_buffer_memory_requirements =
    scegfx_context_vulkan_get_buffer_memory_requirements,
  .bind_buffer_memory = scegfx_context_vulkan_bind_buffer_memory,
  .bind_image_memory = scegfx_context_vulkan_bind_image_memory,
  .create_swapchain = scegfx_context_vulkan_create_swapchain,
  .destroy_swapchain = scegfx_context_vulkan_destroy_swapchain,
  .create_fence = scegfx_context_vulkan_create_fence,
  .destroy_fence = scegfx_context_vulkan_destroy_fence,
  .create_semaphore = scegfx_context_vulkan_create_semaphore,
  .destroy_semaphore = scegfx_context_vulkan_destroy_semaphore,
  .create_image = scegfx_context_vulkan_create_image,
  .destroy_image = scegfx_context_vulkan_destroy_image,
  .get_image_memory_requirements =
    scegfx_context_vulkan_get_image_memory_requirements,
  .create_image_view = scegfx_vulkan_create_image_view,
  .destroy_image_view = scegfx_vulkan_destroy_image_view,
  .create_sampler = scegfx_context_vulkan_create_sampler,
  .destroy_sampler = scegfx_context_vulkan_destroy_sampler,
  .create_render_pass = scegfx_vulkan_create_render_pass,
  .destroy_render_pass = scegfx_vulkan_destroy_render_pass,
  .create_framebuffer = scegfx_vulkan_create_framebuffer,
  .destroy_framebuffer = scegfx_vulkan_destroy_framebuffer,
  .create_command_buffer = scegfx_context_vulkan_create_command_buffer,
  .destroy_command_buffer = scegfx_context_vulkan_destroy_command_buffer,
  .make_current = scegfx_context_vulkan_make_current,
  .submit_to_queue = scegfx_context_vulkan_submit_to_queue,
  .present = scegfx_context_vulkan_present,
  .wait_idle = scegfx_context_vulkan_wait_idle,
};

#endif // SCEGFX_CONTEXT_VULKAN_H

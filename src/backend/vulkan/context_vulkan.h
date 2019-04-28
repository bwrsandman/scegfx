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
  VkDevice device;
  uint32_t present_graphics_compute_transfer_queue_index;
  VkQueue present_graphics_compute_transfer_queue;
  VkCommandPool command_pool;
} scegfx_context_vulkan_t;

bool
scegfx_context_vulkan_initialize(scegfx_context_t* this);
void
scegfx_context_vulkan_terminate(scegfx_context_t* this);

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

bool
scegfx_context_vulkan_make_current(scegfx_context_t* this);

static const scegfx_context_api_vtable_t scegfx_context_api_vtable_vulkan = {
  .initialize = scegfx_context_vulkan_initialize,
  .terminate = scegfx_context_vulkan_terminate,
  .create_swapchain = scegfx_context_vulkan_create_swapchain,
  .destroy_swapchain = scegfx_context_vulkan_destroy_swapchain,
  .create_fence = scegfx_context_vulkan_create_fence,
  .destroy_fence = scegfx_context_vulkan_destroy_fence,
  .create_semaphore = scegfx_context_vulkan_create_semaphore,
  .destroy_semaphore = scegfx_context_vulkan_destroy_semaphore,
  .make_current = scegfx_context_vulkan_make_current,
};

#endif // SCEGFX_CONTEXT_VULKAN_H

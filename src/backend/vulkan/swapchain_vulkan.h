//
// Created by sandy on 24/04/19.
//

#ifndef SCEGFX_SWAPCHAIN_VULKAN_H
#define SCEGFX_SWAPCHAIN_VULKAN_H

#include <scegfx/swapchain.h>

#include <vulkan/vulkan.h>

#include "image_vulkan.h"

enum
{
  SCEGFX_MAX_SWAPCHAIN_COUNT = 3,
};

typedef struct scegfx_swapchain_vulkan_t
{
  scegfx_swapchain_t super;
  VkSwapchainKHR handle;
  scegfx_image_vulkan_t images[SCEGFX_MAX_SWAPCHAIN_COUNT];
  VkDescriptorPool descriptor_pool;
} scegfx_swapchain_vulkan_t;

bool
scegfx_swapchain_vulkan_initialize(scegfx_swapchain_t* this);
void
scegfx_swapchain_vulkan_terminate(scegfx_swapchain_t* this);

scegfx_image_t*
scegfx_swapchain_vulkan_get_image(scegfx_swapchain_t* this, uint32_t index);
bool
scegfx_swapchain_vulkan_acquire_next_image(scegfx_swapchain_t* this,
                                           uint64_t timeout,
                                           scegfx_semaphore_t* semaphore,
                                           scegfx_fence_t* fence,
                                           uint32_t* image_index);
scegfx_descriptor_set_t*
scegfx_swapchain_vulkan_allocate_descriptor_set(
  scegfx_swapchain_t* this,
  const scegfx_descriptor_set_layout_t* layout,
  scegfx_allocator_t* allocator);
void
scegfx_swapchain_vulkan_free_descriptor_set(
  scegfx_swapchain_t* this,
  scegfx_descriptor_set_t* descriptor_set,
  scegfx_allocator_t* allocator);

const static scegfx_swapchain_api_vtable_t
  scegfx_swapchain_api_vtable_vulkan = {
    .initialize = scegfx_swapchain_vulkan_initialize,
    .terminate = scegfx_swapchain_vulkan_terminate,
    .get_image = scegfx_swapchain_vulkan_get_image,
    .acquire_next_image = scegfx_swapchain_vulkan_acquire_next_image,
    .allocate_descriptor_set = scegfx_swapchain_vulkan_allocate_descriptor_set,
    .free_descriptor_set = scegfx_swapchain_vulkan_free_descriptor_set,
  };

#endif // SCEGFX_SWAPCHAIN_VULKAN_H

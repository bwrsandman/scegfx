//
// Created by sandy on 24/04/19.
//

#ifndef SCEGFX_SWAPCHAIN_VULKAN_H
#define SCEGFX_SWAPCHAIN_VULKAN_H

#include <scegfx/swapchain.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_swapchain_vulkan_t
{
  scegfx_swapchain_t super;
  VkSwapchainKHR handle;
} scegfx_swapchain_vulkan_t;

bool
scegfx_swapchain_vulkan_initialize(scegfx_swapchain_t* this);
void
scegfx_swapchain_vulkan_terminate(scegfx_swapchain_t* this);

const static scegfx_swapchain_api_vtable_t
  scegfx_swapchain_api_vtable_vulkan = {
    .initialize = scegfx_swapchain_vulkan_initialize,
    .terminate = scegfx_swapchain_vulkan_terminate,
  };

#endif // SCEGFX_SWAPCHAIN_VULKAN_H

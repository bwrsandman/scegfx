//
// Created by sandy on 28/04/19.
//

#ifndef SCEGFX_FENCE_VULKAN_H
#define SCEGFX_FENCE_VULKAN_H

#include <scegfx/fence.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_fence_vulkan_t
{
  scegfx_fence_t super;
  VkFence handle;
} scegfx_fence_vulkan_t;

bool
scegfx_fence_vulkan_initialize(scegfx_fence_t* this, bool signaled);
void
scegfx_fence_vulkan_terminate(scegfx_fence_t* this);

bool
scegfx_fence_vulkan_wait(scegfx_fence_t* this, uint64_t timeout);
bool
scegfx_fence_vulkan_reset(scegfx_fence_t* this);

const static scegfx_fence_api_vtable_t scegfx_fence_api_vtable_vulkan = {
  .initialize = scegfx_fence_vulkan_initialize,
  .terminate = scegfx_fence_vulkan_terminate,
  .wait = scegfx_fence_vulkan_wait,
  .reset = scegfx_fence_vulkan_reset,
};

#endif // SCEGFX_FENCE_VULKAN_H

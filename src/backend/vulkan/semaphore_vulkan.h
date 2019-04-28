//
// Created by sandy on 28/04/19.
//

#ifndef SCEGFX_SEMAPHORE_VULKAN_H
#define SCEGFX_SEMAPHORE_VULKAN_H

#include <scegfx/semaphore.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_semaphore_vulkan_t
{
  scegfx_semaphore_t super;
  VkSemaphore handle;
} scegfx_semaphore_vulkan_t;

bool
scegfx_semaphore_vulkan_initialize(scegfx_semaphore_t* this);
void
scegfx_semaphore_vulkan_terminate(scegfx_semaphore_t* this);

const static scegfx_semaphore_api_vtable_t
  scegfx_semaphore_api_vtable_vulkan = {
    .initialize = scegfx_semaphore_vulkan_initialize,
    .terminate = scegfx_semaphore_vulkan_terminate,
  };

#endif // SCEGFX_SEMAPHORE_VULKAN_H

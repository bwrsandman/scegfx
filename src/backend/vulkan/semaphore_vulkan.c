//
// Created by sandy on 28/04/19.
//

#include "semaphore_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"

bool
scegfx_semaphore_vulkan_initialize(scegfx_semaphore_t* super)
{
  assert(!super->initialized);
  scegfx_semaphore_vulkan_t* this = (scegfx_semaphore_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkSemaphoreCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };

  VkResult result =
    vkCreateSemaphore(context->device, &info, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create semaphore");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_semaphore_vulkan_terminate(scegfx_semaphore_t* super)
{
  assert(super->initialized);
  scegfx_semaphore_vulkan_t* this = (scegfx_semaphore_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  vkDestroySemaphore(context->device, this->handle, NULL);
  super->initialized = false;
}

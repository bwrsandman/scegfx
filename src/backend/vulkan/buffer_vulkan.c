//
// Created by sandy on 05/05/19.
//

#include "buffer_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"

bool
scegfx_buffer_vulkan_initialize(scegfx_buffer_t* super,
                                scegfx_device_size_t size,
                                scegfx_buffer_usage_t usage)
{
  assert(!super->initialized);
  scegfx_buffer_vulkan_t* this = (scegfx_buffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkBufferCreateInfo vk_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = size,
    .usage = usage,
  };

  VkResult result =
    vkCreateBuffer(context->device, &vk_info, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create buffer");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_buffer_vulkan_terminate(scegfx_buffer_t* super)
{
  assert(super->initialized);
  scegfx_buffer_vulkan_t* this = (scegfx_buffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  vkDestroyBuffer(context->device, this->handle, NULL);
}

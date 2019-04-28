//
// Created by sandy on 28/04/19.
//

#include "fence_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"

bool
scegfx_fence_vulkan_initialize(scegfx_fence_t* super, bool signaled)
{
  assert(!super->initialized);
  scegfx_fence_vulkan_t* this = (scegfx_fence_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkFenceCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0,
  };

  VkResult result = vkCreateFence(context->device, &info, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create fence");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_fence_vulkan_terminate(scegfx_fence_t* super)
{
  assert(super->initialized);
  scegfx_fence_vulkan_t* this = (scegfx_fence_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  super->api_vtable->wait(super, UINT64_MAX);
  vkDestroyFence(context->device, this->handle, NULL);
  super->initialized = false;
}

bool
scegfx_fence_vulkan_wait(scegfx_fence_t* super, uint64_t timeout)
{
  assert(super->initialized);
  scegfx_fence_vulkan_t* this = (scegfx_fence_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkResult result =
    vkWaitForFences(context->device, 1, &this->handle, VK_TRUE, timeout);

  switch (result) {
    case VK_SUCCESS:
      return true;
    case VK_TIMEOUT:
      context->super.debug_callback(scegfx_debug_severity_warning,
                                    __LINE__,
                                    FILE_BASENAME,
                                    "Timeout waiting for fence");
      return false;
    default:
      context->super.debug_callback(scegfx_debug_severity_error,
                                    __LINE__,
                                    FILE_BASENAME,
                                    "Unable to wait for fence");
      assert(false);
      return false;
  }
}

bool
scegfx_fence_vulkan_reset(scegfx_fence_t* super)
{
  assert(super->initialized);
  scegfx_fence_vulkan_t* this = (scegfx_fence_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkResult result = vkResetFences(context->device, 1, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to reset fence");
    return false;
  }

  return true;
}

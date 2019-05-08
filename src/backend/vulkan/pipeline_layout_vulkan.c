//
// Created by sandy on 06/05/19.
//

#include "pipeline_layout_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"

bool
scegfx_pipeline_layout_vulkan_initialize(scegfx_pipeline_layout_t* super)
{
  assert(!super->initialized);
  scegfx_pipeline_layout_vulkan_t* this =
    (scegfx_pipeline_layout_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkPipelineLayoutCreateInfo vk_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
  };

  VkResult result =
    vkCreatePipelineLayout(context->device, &vk_info, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create pipeline layout");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_pipeline_layout_vulkan_terminate(scegfx_pipeline_layout_t* super)
{
  assert(super->initialized);
  scegfx_pipeline_layout_vulkan_t* this =
    (scegfx_pipeline_layout_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  vkDestroyPipelineLayout(context->device, this->handle, NULL);
}

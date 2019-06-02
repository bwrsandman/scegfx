//
// Created by sandy on 06/05/19.
//

#include "pipeline_layout_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"
#include "descriptor_set_layout_vulkan.h"

bool
scegfx_pipeline_layout_vulkan_initialize(
  scegfx_pipeline_layout_t* super,
  const scegfx_pipeline_layout_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_pipeline_layout_vulkan_t* this =
    (scegfx_pipeline_layout_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkDescriptorSetLayout set_layouts[16];
  assert(info->set_layout_count <=
         sizeof(set_layouts) / sizeof(set_layouts[0]));
  for (uint32_t i = 0; i < info->set_layout_count; ++i) {
    scegfx_descriptor_set_layout_vulkan_t* vk_set_layout =
      (scegfx_descriptor_set_layout_vulkan_t*)info->set_layouts[i];
    set_layouts[i] = vk_set_layout->handle;
  }

  VkPipelineLayoutCreateInfo vk_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = info->set_layout_count,
    .pSetLayouts = set_layouts,
    .pushConstantRangeCount = info->push_constant_range_count,
    .pPushConstantRanges =
      (const VkPushConstantRange*)info->push_constant_ranges,
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

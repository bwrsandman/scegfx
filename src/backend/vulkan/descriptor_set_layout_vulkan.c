//
// Created by sandy on 31/05/19.
//

#include "descriptor_set_layout_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"

bool
scegfx_descriptor_set_layout_vulkan_initialize(
  struct scegfx_descriptor_set_layout_t* super,
  const scegfx_descriptor_set_layout_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_descriptor_set_layout_vulkan_t* this =
    (scegfx_descriptor_set_layout_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkDescriptorSetLayoutCreateInfo vk_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .bindingCount = info->binding_count,
    .pBindings = (const VkDescriptorSetLayoutBinding*)info->bindings,
  };

  VkResult result =
    vkCreateDescriptorSetLayout(context->device, &vk_info, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create descriptor set layout");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_descriptor_set_layout_vulkan_terminate(
  scegfx_descriptor_set_layout_t* super)
{
  assert(super->initialized);
  scegfx_descriptor_set_layout_vulkan_t* this =
    (scegfx_descriptor_set_layout_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  vkDestroyDescriptorSetLayout(context->device, this->handle, NULL);
  super->initialized = false;
}

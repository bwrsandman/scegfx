//
// Created by sandy on 08/06/19.
//

#include "image_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"

bool
scegfx_image_vulkan_initialize(scegfx_image_t* super,
                               const scegfx_image_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_image_vulkan_t* this = (scegfx_image_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkImageCreateInfo vk_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType = (VkImageType)info->image_type,
    .format = (VkFormat)info->format,
    .extent = *(VkExtent3D*)&info->extent,
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = (VkImageTiling)info->tiling,
    .usage = info->usage,
    .initialLayout = (VkImageLayout)info->initial_layout,
  };

  VkResult result =
    vkCreateImage(context->device, &vk_info, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create image");
    return false;
  }
  this->super.type = info->image_type;
  this->super.format = info->format;
  this->super.extent = info->extent;
  this->super.initialized = true;
  return true;
}

void
scegfx_image_vulkan_terminate(scegfx_image_t* super)
{
  assert(super->initialized);
  scegfx_image_vulkan_t* this = (scegfx_image_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  vkDestroyImage(context->device, this->handle, NULL);
}

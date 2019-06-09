//
// Created by sandy on 09/06/19.
//

#include "image_view_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"
#include "image_vulkan.h"

bool
scegfx_image_view_vulkan_initialize(scegfx_image_view_t* super,
                                    const scegfx_image_t* image,
                                    scegfx_image_aspect_t aspect)
{
  assert(!super->initialized);
  scegfx_image_view_vulkan_t* this = (scegfx_image_view_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  const scegfx_image_vulkan_t* image_vk = (const scegfx_image_vulkan_t*)image;

  VkImageViewCreateInfo view_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = image_vk->handle,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = (VkFormat)image_vk->super.format,
      .components = {
          .r = VK_COMPONENT_SWIZZLE_R,
          .g = VK_COMPONENT_SWIZZLE_G,
          .b = VK_COMPONENT_SWIZZLE_B,
          .a = VK_COMPONENT_SWIZZLE_A,
      },
      .subresourceRange = {
          .aspectMask = (VkImageAspectFlagBits)aspect,
          .levelCount = 1,
          .layerCount = 1,
      }
  };
  VkResult res =
    vkCreateImageView(context->device, &view_info, NULL, &this->handle);
  if (res != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable create swapchain image views");
    return false;
  }
  this->super.extent = image_vk->super.extent;
  this->super.initialized = true;
  return true;
}

void
scegfx_image_view_vulkan_terminate(scegfx_image_view_t* super)
{
  assert(super->initialized);
  scegfx_image_view_vulkan_t* this = (scegfx_image_view_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  vkDestroyImageView(context->device, this->handle, NULL);
}

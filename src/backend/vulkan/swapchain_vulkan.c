//
// Created by sandy on 24/04/19.
//

#include "swapchain_vulkan.h"

#include <assert.h>

#include <SDL_vulkan.h>

#include "context_vulkan.h"

bool
scegfx_swapchain_vulkan_initialize(scegfx_swapchain_t* super)
{
  assert(!super->initialized);
  scegfx_swapchain_vulkan_t* this = (scegfx_swapchain_vulkan_t*)super;
  const scegfx_context_vulkan_t* context =
    (const scegfx_context_vulkan_t*)super->context;
  int width, height;
  SDL_Vulkan_GetDrawableSize(context->super.window_handle, &width, &height);
  if (context->surface_capabilities.currentExtent.width == 0xFFFFFFFF) {
    this->super.extent.width = (uint32_t)width;
    this->super.extent.height = (uint32_t)height;
    if (this->super.extent.width <
        context->surface_capabilities.minImageExtent.width) {
      this->super.extent.width =
        context->surface_capabilities.minImageExtent.width;
    } else if (this->super.extent.width >
               context->surface_capabilities.maxImageExtent.width) {
      this->super.extent.width =
        context->surface_capabilities.maxImageExtent.width;
    }

    if (this->super.extent.height <
        context->surface_capabilities.minImageExtent.height) {
      this->super.extent.height =
        context->surface_capabilities.minImageExtent.height;
    } else if (this->super.extent.height >
               context->surface_capabilities.maxImageExtent.height) {
      this->super.extent.height =
        context->surface_capabilities.maxImageExtent.height;
    }
  } else {
    // If the surface size is defined, the swap chain size must match
    this->super.extent.width =
      context->surface_capabilities.currentExtent.width;
    this->super.extent.height =
      context->surface_capabilities.currentExtent.height;
  }
  width = context->surface_capabilities.currentExtent.width;
  height = context->surface_capabilities.currentExtent.height;

  VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
  for (uint32_t i = 0; i < context->surface_present_mode_count; ++i) {
    if (context->surface_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    }
    if (context->surface_present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
  }

  uint32_t desiredNumberOfSwapchainImages =
    context->surface_capabilities.minImageCount + 1;
  if ((context->surface_capabilities.maxImageCount > 0) &&
      (desiredNumberOfSwapchainImages >
       context->surface_capabilities.maxImageCount)) {
    desiredNumberOfSwapchainImages =
      context->surface_capabilities.maxImageCount;
  }

  VkSurfaceTransformFlagBitsKHR pre_transform;
  if (context->surface_capabilities.supportedTransforms &
      VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
    pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  } else {
    pre_transform = context->surface_capabilities.currentTransform;
  }

  VkSwapchainCreateInfoKHR info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = context->surface,
      .minImageCount = desiredNumberOfSwapchainImages,
      .imageFormat = context->surface_format.format,
      .imageColorSpace = context->surface_format.colorSpace,
      .imageExtent = {
          .width = this->super.extent.width,
          .height = this->super.extent.height,
      },
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .preTransform = pre_transform,
      .imageArrayLayers = 1,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .presentMode = swapchainPresentMode,
      .clipped = VK_TRUE,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
  };
  VkResult res =
    vkCreateSwapchainKHR(context->device, &info, NULL, &this->handle);
  if (res != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create swapchain");
    return false;
  }
  res = vkGetSwapchainImagesKHR(
    context->device, this->handle, &this->super.image_count, NULL);
  if (res != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to get swapchain count");
    return false;
  }
  this->super.initialized = true;
  return true;
}

void
scegfx_swapchain_vulkan_terminate(scegfx_swapchain_t* super)
{
  assert(super->initialized);
  scegfx_swapchain_vulkan_t* this = (scegfx_swapchain_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  vkDestroySwapchainKHR(context->device, this->handle, NULL);
  super->initialized = false;
}

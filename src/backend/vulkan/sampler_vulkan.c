//
// Created by sandy on 25/07/19.
//

#include "sampler_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"

bool
scegfx_sampler_vulkan_initialize(scegfx_sampler_t* super,
                                 const scegfx_sampler_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_sampler_vulkan_t* this = (scegfx_sampler_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkSamplerCreateInfo vk_info = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .magFilter = (VkFilter)info->mag_filter,
    .minFilter = (VkFilter)info->min_filter,
    .mipmapMode = (VkSamplerMipmapMode)info->mipmap_mode,
    .addressModeU = (VkSamplerAddressMode)info->address_mode_u,
    .addressModeV = (VkSamplerAddressMode)info->address_mode_v,
    .addressModeW = (VkSamplerAddressMode)info->address_mode_w,
  };

  VkResult result =
    vkCreateSampler(context->device, &vk_info, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create sampler");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_sampler_vulkan_terminate(scegfx_sampler_t* super)
{
  assert(super->initialized);
  scegfx_sampler_vulkan_t* this = (scegfx_sampler_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  vkDestroySampler(context->device, this->handle, NULL);
}

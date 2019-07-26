//
// Created by sandy on 25/07/19.
//

#ifndef SCEGFX_SAMPLER_VULKAN_H
#define SCEGFX_SAMPLER_VULKAN_H

#include <scegfx/sampler.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_sampler_vulkan_t
{
  scegfx_sampler_t super;
  VkSampler handle;
} scegfx_sampler_vulkan_t;

bool
scegfx_sampler_vulkan_initialize(scegfx_sampler_t* this,
                                 const scegfx_sampler_create_info_t* info);
void
scegfx_sampler_vulkan_terminate(scegfx_sampler_t* this);

const static scegfx_sampler_api_vtable_t scegfx_sampler_api_vtable_vulkan = {
  .initialize = scegfx_sampler_vulkan_initialize,
  .terminate = scegfx_sampler_vulkan_terminate,
};

#endif // SCEGFX_SAMPLER_VULKAN_H

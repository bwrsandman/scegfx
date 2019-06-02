//
// Created by sandy on 01/06/19.
//

#ifndef SCEGFX_DESCRIPTOR_SET_VULKAN_H
#define SCEGFX_DESCRIPTOR_SET_VULKAN_H

#include <scegfx/descriptor_set.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_descriptor_set_vulkan_t
{
  scegfx_descriptor_set_t super;
  VkDescriptorSet handle;
} scegfx_descriptor_set_vulkan_t;

#endif // SCEGFX_DESCRIPTOR_SET_VULKAN_H

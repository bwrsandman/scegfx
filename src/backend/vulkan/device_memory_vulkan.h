//
// Created by sandy on 05/05/19.
//

#ifndef SCEGFX_MEMORY_VULKAN_H
#define SCEGFX_MEMORY_VULKAN_H

#include <scegfx/device_memory.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_device_memory_vulkan_t
{
  scegfx_device_memory_t super;
  VkDeviceMemory handle;
} scegfx_device_memory_vulkan_t;

#endif // SCEGFX_MEMORY_VULKAN_H

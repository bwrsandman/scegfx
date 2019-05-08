//
// Created by sandy on 05/05/19.
//

#ifndef SCEGFX_BUFFER_VULKAN_H
#define SCEGFX_BUFFER_VULKAN_H

#include <scegfx/buffer.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_buffer_vulkan_t
{
  scegfx_buffer_t super;
  VkBuffer handle;
} scegfx_buffer_vulkan_t;

bool
scegfx_buffer_vulkan_initialize(scegfx_buffer_t* this,
                                scegfx_device_size_t size,
                                scegfx_buffer_usage_t usage);
void
scegfx_buffer_vulkan_terminate(scegfx_buffer_t* this);

const static scegfx_buffer_api_vtable_t scegfx_buffer_api_vtable_vulkan = {
  .initialize = scegfx_buffer_vulkan_initialize,
  .terminate = scegfx_buffer_vulkan_terminate,
};

#endif // SCEGFX_BUFFER_VULKAN_H

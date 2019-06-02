//
// Created by sandy on 05/05/19.
//

#ifndef SCEGFX_DESCRIPTOR_SET_LAYOUT_VULKAN_H
#define SCEGFX_DESCRIPTOR_SET_LAYOUT_VULKAN_H

#include <scegfx/descriptor_set_layout.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_descriptor_set_layout_vulkan_t
{
  struct scegfx_descriptor_set_layout_t super;
  VkDescriptorSetLayout handle;
} scegfx_descriptor_set_layout_vulkan_t;

bool
scegfx_descriptor_set_layout_vulkan_initialize(
  scegfx_descriptor_set_layout_t* this,
  const scegfx_descriptor_set_layout_create_info_t* info);
void
scegfx_descriptor_set_layout_vulkan_terminate(
  scegfx_descriptor_set_layout_t* this);

const static scegfx_descriptor_set_layout_api_vtable_t
  scegfx_descriptor_set_layout_api_vtable_vulkan = {
    .initialize = scegfx_descriptor_set_layout_vulkan_initialize,
    .terminate = scegfx_descriptor_set_layout_vulkan_terminate,
  };

#endif // SCEGFX_DESCRIPTOR_SET_LAYOUT_VULKAN_H

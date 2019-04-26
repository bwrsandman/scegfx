//
// Created by sandy on 25/04/19.
//

#ifndef SCEGFX_IMAGE_VULKAN_H
#define SCEGFX_IMAGE_VULKAN_H

#include <scegfx/image.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_image_vulkan_t
{
  scegfx_image_t super;
  VkImage handle;
} scegfx_image_vulkan_t;

bool
scegfx_image_vulkan_initialize(scegfx_image_t* this,
                               const scegfx_image_create_info_t* info);
void
scegfx_image_vulkan_terminate(scegfx_image_t* this);

const static scegfx_image_api_vtable_t scegfx_image_api_vtable_vulkan = {
  .initialize = scegfx_image_vulkan_initialize,
  .terminate = scegfx_image_vulkan_terminate,
};

#endif // SCEGFX_IMAGE_VULKAN_H

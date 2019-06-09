//
// Created by sandy on 09/06/19.
//

#ifndef SCEGFX_IMAGE_VIEW_VULKAN_H
#define SCEGFX_IMAGE_VIEW_VULKAN_H

#include <scegfx/image_view.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_image_view_vulkan_t
{
  scegfx_image_view_t super;
  VkImageView handle;
} scegfx_image_view_vulkan_t;

bool
scegfx_image_view_vulkan_initialize(scegfx_image_view_t* this,
                                    const scegfx_image_t* image,
                                    scegfx_image_aspect_t aspect);
void
scegfx_image_view_vulkan_terminate(scegfx_image_view_t* this);

const static scegfx_image_view_api_vtable_t
  scegfx_image_view_api_vtable_vulkan = {
    .initialize = scegfx_image_view_vulkan_initialize,
    .terminate = scegfx_image_view_vulkan_terminate,
  };

#endif // SCEGFX_IMAGE_VIEW_VULKAN_H

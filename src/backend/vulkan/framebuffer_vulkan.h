//
// Created by sandy on 27/04/19.
//

#ifndef SCEGFX_FRAMEBUFFER_VULKAN_H
#define SCEGFX_FRAMEBUFFER_VULKAN_H

#include <scegfx/framebuffer.h>

#include <vulkan/vulkan.h>

enum
{
  SCEGFX_VULKAN_MAX_FRAMEBUFFER_ATTACHMENT_COUNT = 2,
};

typedef struct scegfx_framebuffer_vulkan_t
{
  scegfx_framebuffer_t super;
  VkFramebuffer handle;
} scegfx_framebuffer_vulkan_t;

bool
scegfx_framebuffer_vulkan_initialize(
  scegfx_framebuffer_t* this,
  const scegfx_framebuffer_create_info_t* info);
void
scegfx_framebuffer_vulkan_terminate(scegfx_framebuffer_t* this);

const static scegfx_framebuffer_api_vtable_t
  scegfx_framebuffer_api_vtable_vulkan = {
    .initialize = scegfx_framebuffer_vulkan_initialize,
    .terminate = scegfx_framebuffer_vulkan_terminate,
  };

#endif // SCEGFX_FRAMEBUFFER_VULKAN_H

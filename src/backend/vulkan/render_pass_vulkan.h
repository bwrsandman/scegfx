//
// Created by sandy on 25/04/19.
//

#ifndef SCEGFX_RENDER_PASS_VULKAN_H
#define SCEGFX_RENDER_PASS_VULKAN_H

#include <scegfx/render_pass.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_render_pass_vulkan_t
{
  scegfx_render_pass_t super;
  VkRenderPass handle;
} scegfx_render_pass_vulkan_t;

bool
scegfx_render_pass_vulkan_initialize(scegfx_render_pass_t* this);
void
scegfx_render_pass_vulkan_terminate(scegfx_render_pass_t* this);

const static scegfx_render_pass_api_vtable_t
  scegfx_render_pass_api_vtable_vulkan = {
    .initialize = scegfx_render_pass_vulkan_initialize,
    .terminate = scegfx_render_pass_vulkan_terminate,
  };

#endif // SCEGFX_RENDER_PASS_VULKAN_H

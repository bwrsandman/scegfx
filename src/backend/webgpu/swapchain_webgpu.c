//
// Created by sandy on 19/06/19.
//

#include "swapchain_webgpu.h"

#include <assert.h>

#include <SDL_vulkan.h>

#include "context_webgpu.h"

bool
scegfx_swapchain_webgpu_initialize(scegfx_swapchain_t* super)
{
  assert(!super->initialized);
  scegfx_swapchain_webgpu_t* this = (scegfx_swapchain_webgpu_t*)super;
  const scegfx_context_webgpu_t* context =
    (const scegfx_context_webgpu_t*)super->context;

  int width, height;
  SDL_Vulkan_GetDrawableSize(context->super.window_handle, &width, &height);

  WGPUSwapChainDescriptor desc = {
    .usage = WGPUTextureUsage_OUTPUT_ATTACHMENT,
    .format = WGPUTextureFormat_Bgra8Unorm,
    .width = width,
    .height = height,
  };
  this->handle =
    wgpu_device_create_swap_chain(context->device, context->surface, &desc);

  return true;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void
scegfx_swapchain_webgpu_terminate(scegfx_swapchain_t* super)
{}
#pragma clang diagnostic pop

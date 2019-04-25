//
// Created by sandy on 19/06/19.
//

#ifndef SCEGFX_SWAPCHAIN_WEBGPU_H
#define SCEGFX_SWAPCHAIN_WEBGPU_H

#include <scegfx/swapchain.h>

#include <wgpu/wgpu.h>

typedef struct scegfx_swapchain_webgpu_t
{
  scegfx_swapchain_t super;
  WGPUSwapChainId handle;
} scegfx_swapchain_webgpu_t;

bool
scegfx_swapchain_webgpu_initialize(scegfx_swapchain_t* this);
void
scegfx_swapchain_webgpu_terminate(scegfx_swapchain_t* this);

const static scegfx_swapchain_api_vtable_t
  scegfx_swapchain_api_vtable_webgpu = {
    .initialize = scegfx_swapchain_webgpu_initialize,
    .terminate = scegfx_swapchain_webgpu_terminate,
  };

#endif // SCEGFX_SWAPCHAIN_WEBGPU_H

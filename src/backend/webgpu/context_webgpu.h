//
// Created by sandy on 19/06/19.
//

#ifndef SCEGFX_CONTEXT_WEBGPU_H
#define SCEGFX_CONTEXT_WEBGPU_H

#include <scegfx/context.h>

#include <wgpu/wgpu.h>

typedef struct scegfx_context_webgpu_t
{
  scegfx_context_t super;
  WGPUInstanceId instance;
  WGPUAdapterId adapter;
  WGPUDeviceId device;
  WGPUSurfaceId surface;
} scegfx_context_webgpu_t;

bool
scegfx_context_webgpu_initialize(scegfx_context_t* this);
void
scegfx_context_webgpu_terminate(scegfx_context_t* this);

scegfx_swapchain_t*
scegfx_context_webgpu_create_swapchain(scegfx_context_t* this,
                                       scegfx_allocator_t* allocator);
void
scegfx_context_webgpu_destroy_swapchain(scegfx_context_t* this,
                                        scegfx_swapchain_t* swapchain,
                                        scegfx_allocator_t* allocator);

bool
scegfx_context_webgpu_make_current(scegfx_context_t* this);

static const scegfx_context_api_vtable_t scegfx_context_api_vtable_webgpu = {
  .initialize = scegfx_context_webgpu_initialize,
  .terminate = scegfx_context_webgpu_terminate,
  .create_swapchain = scegfx_context_webgpu_create_swapchain,
  .destroy_swapchain = scegfx_context_webgpu_destroy_swapchain,
  .make_current = scegfx_context_webgpu_make_current,
};

#endif // SCEGFX_CONTEXT_WEBGPU_H

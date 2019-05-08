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

uint32_t
scegfx_context_webgpu_get_memory_type(scegfx_context_t* this,
                                      uint32_t type_bits,
                                      scegfx_memory_properties_t properties);
scegfx_device_memory_t*
scegfx_context_webgpu_allocate_memory(
  scegfx_context_t* this,
  const scegfx_device_memory_allocate_info_t* info,
  scegfx_allocator_t* allocator);
void
scegfx_context_webgpu_free_memory(scegfx_context_t* this,
                                  scegfx_device_memory_t* memory,
                                  scegfx_allocator_t* allocator);
bool
scegfx_context_webgpu_map_memory(scegfx_context_t* this,
                                 scegfx_device_memory_t* memory,
                                 scegfx_device_size_t offset,
                                 scegfx_device_size_t size,
                                 void** data);
void
scegfx_context_webgpu_unmap_memory(scegfx_context_t* this,
                                   scegfx_device_memory_t* memory);

bool
scegfx_context_webgpu_flush_mapped_memory_ranges(
  scegfx_context_t* this,
  uint32_t memory_range_count,
  const scegfx_mapped_device_memory_range_t* memory_ranges);

scegfx_buffer_t*
scegfx_context_webgpu_create_buffer(scegfx_context_t* this,
                                    scegfx_allocator_t* allocator);
void
scegfx_context_webgpu_destroy_buffer(scegfx_context_t* this,
                                     scegfx_buffer_t* buffer,
                                     scegfx_allocator_t* allocator);
void
scegfx_context_webgpu_get_buffer_memory_requirements(
  scegfx_context_t* this,
  const scegfx_buffer_t* buffer,
  scegfx_device_memory_requirements_t* memory_requirements);

bool
scegfx_context_webgpu_bind_buffer_memory(scegfx_context_t* this,
                                         scegfx_buffer_t* buffer,
                                         scegfx_device_memory_t* memory,
                                         scegfx_device_size_t memory_offset);

bool
scegfx_context_webgpu_make_current(scegfx_context_t* this);

static const scegfx_context_api_vtable_t scegfx_context_api_vtable_webgpu = {
  .initialize = scegfx_context_webgpu_initialize,
  .terminate = scegfx_context_webgpu_terminate,
  .create_swapchain = scegfx_context_webgpu_create_swapchain,
  .destroy_swapchain = scegfx_context_webgpu_destroy_swapchain,
  .get_memory_type = scegfx_context_webgpu_get_memory_type,
  .allocate_memory = scegfx_context_webgpu_allocate_memory,
  .free_memory = scegfx_context_webgpu_free_memory,
  .map_memory = scegfx_context_webgpu_map_memory,
  .unmap_memory = scegfx_context_webgpu_unmap_memory,
  .flush_mapped_memory_ranges =
    scegfx_context_webgpu_flush_mapped_memory_ranges,
  .create_buffer = scegfx_context_webgpu_create_buffer,
  .destroy_buffer = scegfx_context_webgpu_destroy_buffer,
  .get_buffer_memory_requirements =
    scegfx_context_webgpu_get_buffer_memory_requirements,
  .bind_buffer_memory = scegfx_context_webgpu_bind_buffer_memory,
  .make_current = scegfx_context_webgpu_make_current,
};

#endif // SCEGFX_CONTEXT_WEBGPU_H

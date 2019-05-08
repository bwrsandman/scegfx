//
// Created by sandy on 19/06/19.
//

#include "context_webgpu.h"

#include <assert.h>

#include <SDL_syswm.h>

#include "buffer_webgpu.h"
#include "swapchain_webgpu.h"

bool
scegfx_context_webgpu_initialize(scegfx_context_t* super)
{
  assert(!super->initialized);
  scegfx_context_webgpu_t* this = (scegfx_context_webgpu_t*)super;
  this->instance = wgpu_create_instance();

  this->adapter = wgpu_instance_get_adapter(
    this->instance,
    &(WGPUAdapterDescriptor){
      .power_preference = WGPUPowerPreference_LowPower,
    });

  this->device = wgpu_adapter_request_device(
      this->adapter,
      &(WGPUDeviceDescriptor){
          .extensions = {
              .anisotropic_filtering = false,
          },
  });

  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  if (!SDL_GetWindowWMInfo(this->super.window_handle, &info))
    return false;
  switch (info.subsystem) {
#if defined(SDL_VIDEO_DRIVER_X11)
    case SDL_SYSWM_X11:
      this->surface = wgpu_instance_create_surface_from_xlib(
        this->instance,
        (const void**)info.info.x11.display,
        info.info.x11.window);
      break;
#endif
    default:
      return false;
  }

  this->super.initialized = true;
  return true;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void
scegfx_context_webgpu_terminate(scegfx_context_t* this)
{}

bool
scegfx_context_webgpu_make_current(scegfx_context_t* this)
{
  return true;
}
#pragma clang diagnostic pop

scegfx_swapchain_t*
scegfx_context_webgpu_create_swapchain(scegfx_context_t* super,
                                       scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_swapchain_t* swapchain = NULL;
  if (allocator == NULL)
    swapchain = malloc(sizeof(scegfx_swapchain_webgpu_t));
  else
    swapchain = allocator->allocator_callback(
      NULL, sizeof(scegfx_swapchain_webgpu_t), allocator->user_data);
  memset(swapchain, 0, sizeof(scegfx_swapchain_webgpu_t));

  swapchain->api_vtable = &scegfx_swapchain_api_vtable_webgpu;
  swapchain->context = super;

  return swapchain;
}

void
scegfx_context_webgpu_destroy_swapchain(scegfx_context_t* this,
                                        scegfx_swapchain_t* swapchain,
                                        scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(swapchain);
  } else {
    allocator->allocator_callback(swapchain, 0, allocator->user_data);
  }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
uint32_t
scegfx_context_webgpu_get_memory_type(scegfx_context_t* super,
                                      uint32_t type_bits,
                                      scegfx_memory_properties_t properties)
{
  return 0;
}

scegfx_device_memory_t*
scegfx_context_webgpu_allocate_memory(
  scegfx_context_t* super,
  const scegfx_device_memory_allocate_info_t* info,
  scegfx_allocator_t* allocator)
{
  return NULL;
}

void
scegfx_context_webgpu_free_memory(scegfx_context_t* this,
                                  scegfx_device_memory_t* memory,
                                  scegfx_allocator_t* allocator)
{}

bool
scegfx_context_webgpu_map_memory(scegfx_context_t* this,
                                 scegfx_device_memory_t* memory,
                                 scegfx_device_size_t offset,
                                 scegfx_device_size_t size,
                                 void** data)
{
  return 0;
}

void
scegfx_context_webgpu_unmap_memory(scegfx_context_t* this,
                                   scegfx_device_memory_t* memory)
{}

bool
scegfx_context_webgpu_flush_mapped_memory_ranges(
  scegfx_context_t* this,
  uint32_t memory_range_count,
  const scegfx_mapped_device_memory_range_t* memory_ranges)
{
  return 0;
}

scegfx_buffer_t*
scegfx_context_webgpu_create_buffer(scegfx_context_t* this,
                                    scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  scegfx_buffer_t* buffer = NULL;
  if (allocator == NULL)
    buffer = malloc(sizeof(scegfx_buffer_webgpu_t));
  else
    buffer = allocator->allocator_callback(
      NULL, sizeof(scegfx_buffer_webgpu_t), allocator->user_data);
  memset(buffer, 0, sizeof(scegfx_buffer_webgpu_t));

  buffer->api_vtable = &scegfx_buffer_api_vtable_webgpu;
  buffer->context = this;

  return buffer;
}

void
scegfx_context_webgpu_destroy_buffer(scegfx_context_t* this,
                                     scegfx_buffer_t* buffer,
                                     scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(buffer);
  } else {
    allocator->allocator_callback(buffer, 0, allocator->user_data);
  }
}

void
scegfx_context_webgpu_get_buffer_memory_requirements(
  scegfx_context_t* this,
  const scegfx_buffer_t* buffer,
  scegfx_device_memory_requirements_t* memory_requirements)
{}

bool
scegfx_context_webgpu_bind_buffer_memory(scegfx_context_t* this,
                                         scegfx_buffer_t* buffer,
                                         scegfx_device_memory_t* memory,
                                         scegfx_device_size_t memory_offset)
{
  return 0;
}
#pragma clang diagnostic pop

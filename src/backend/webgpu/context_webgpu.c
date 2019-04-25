//
// Created by sandy on 19/06/19.
//

#include "context_webgpu.h"

#include <assert.h>

#include <SDL_syswm.h>

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

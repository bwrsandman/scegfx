//
// Created by sandy on 19/06/19.
//

#include "context_webgpu.h"

#include <assert.h>

#include <SDL_syswm.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

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

void
scegfx_context_webgpu_terminate(scegfx_context_t* this)
{}

bool
scegfx_context_webgpu_make_current(scegfx_context_t* this)
{
  return true;
}

#pragma clang diagnostic pop

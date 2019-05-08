//
// Created by sandy on 19/06/19.
//

#ifndef SCEGFX_BUFFER_WEBGPU_H
#define SCEGFX_BUFFER_WEBGPU_H

#include <scegfx/buffer.h>

#include <wgpu/wgpu.h>

typedef struct scegfx_buffer_webgpu_t
{
  scegfx_buffer_t super;
  WGPUBufferId handle;
} scegfx_buffer_webgpu_t;

bool
scegfx_buffer_webgpu_initialize(scegfx_buffer_t* this,
                                scegfx_device_size_t size,
                                scegfx_buffer_usage_t usage);
void
scegfx_buffer_webgpu_terminate(scegfx_buffer_t* this);

const static scegfx_buffer_api_vtable_t scegfx_buffer_api_vtable_webgpu = {
  .initialize = scegfx_buffer_webgpu_initialize,
  .terminate = scegfx_buffer_webgpu_terminate,
};

#endif // SCEGFX_BUFFER_WEBGPU_H

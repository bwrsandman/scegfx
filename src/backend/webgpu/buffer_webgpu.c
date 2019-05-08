#include "buffer_webgpu.h"

#include <assert.h>

#include "context_webgpu.h"

bool
scegfx_buffer_webgpu_initialize(scegfx_buffer_t* super,
                                scegfx_device_size_t size,
                                scegfx_buffer_usage_t usage)
{
  assert(!super->initialized);
  scegfx_buffer_webgpu_t* this = (scegfx_buffer_webgpu_t*)super;
  const scegfx_context_webgpu_t* context =
    (const scegfx_context_webgpu_t*)super->context;

  WGPUBufferUsage usage_webgpu = WGPUBufferUsage_NONE;

  if (usage & scegfx_buffer_usage_transfer_src) {
    usage_webgpu |= WGPUBufferUsage_TRANSFER_SRC;
  }
  if (usage & scegfx_buffer_usage_transfer_dst) {
    usage_webgpu |= WGPUBufferUsage_TRANSFER_DST;
  }
  if (usage & scegfx_buffer_usage_uniform_buffer) {
    usage_webgpu |= WGPUBufferUsage_UNIFORM;
  }
  if (usage & scegfx_buffer_usage_storage_buffer) {
    usage_webgpu |= WGPUBufferUsage_STORAGE;
  }
  if (usage & scegfx_buffer_usage_index_buffer) {
    usage_webgpu |= WGPUBufferUsage_INDEX;
  }
  if (usage & scegfx_buffer_usage_vertex_buffer) {
    usage_webgpu |= WGPUBufferUsage_VERTEX;
  }

  WGPUBufferDescriptor desc = {
    .size = size,
    .usage = usage_webgpu,
  };

  this->handle = wgpu_device_create_buffer(context->device, &desc);

  return 0;
}

void
scegfx_buffer_webgpu_terminate(scegfx_buffer_t* super)
{
  assert(super->initialized);
  scegfx_buffer_webgpu_t* this = (scegfx_buffer_webgpu_t*)super;

  wgpu_buffer_destroy(this->handle);
}

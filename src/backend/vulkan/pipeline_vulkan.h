//
// Created by sandy on 06/05/19.
//

#ifndef SCEGFX_PIPELINE_VULKAN_H
#define SCEGFX_PIPELINE_VULKAN_H

#include <scegfx/pipeline.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_pipeline_vulkan_t
{
  scegfx_pipeline_t super;
  VkPipeline handle;
} scegfx_pipeline_vulkan_t;

bool
scegfx_pipeline_vulkan_initialize(scegfx_pipeline_t* this,
                                  const scegfx_pipeline_create_info_t* info);
void
scegfx_pipeline_vulkan_terminate(scegfx_pipeline_t* this);

const static scegfx_pipeline_api_vtable_t scegfx_pipeline_api_vtable_vulkan = {
  .initialize = scegfx_pipeline_vulkan_initialize,
  .terminate = scegfx_pipeline_vulkan_terminate,
};

#endif // SCEGFX_PIPELINE_VULKAN_H

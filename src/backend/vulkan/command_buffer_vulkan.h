//
// Created by sandy on 20/04/19.
//

#ifndef SCEGFX_COMMAND_BUFFER_VULKAN_H
#define SCEGFX_COMMAND_BUFFER_VULKAN_H

#include <scegfx/command_buffer.h>

#include <vulkan/vulkan.h>

typedef struct scegfx_command_buffer_vulkan_t
{
  scegfx_command_buffer_t super;
  VkCommandBuffer handle;
} scegfx_command_buffer_vulkan_t;

bool
scegfx_command_buffer_vulkan_initialize(scegfx_command_buffer_t* this);
void
scegfx_command_buffer_vulkan_terminate(scegfx_command_buffer_t* this);

bool
scegfx_command_buffer_vulkan_begin(scegfx_command_buffer_t* this,
                                   bool long_lived);
bool
scegfx_command_buffer_vulkan_end(scegfx_command_buffer_t* this);

void
scegfx_command_buffer_vulkan_begin_render_pass(
  scegfx_command_buffer_t* this,
  const scegfx_render_pass_begin_info_t* info);
void
scegfx_command_buffer_vulkan_end_render_pass(scegfx_command_buffer_t* this);

void
scegfx_command_buffer_vulkan_bind_pipeline(scegfx_command_buffer_t* this,
                                           scegfx_pipeline_type_t type,
                                           const scegfx_pipeline_t* pipeline);

const static scegfx_command_buffer_api_vtable_t
  scegfx_command_buffer_api_vtable_vulkan = {
    .initialize = scegfx_command_buffer_vulkan_initialize,
    .terminate = scegfx_command_buffer_vulkan_terminate,
    .begin = scegfx_command_buffer_vulkan_begin,
    .end = scegfx_command_buffer_vulkan_end,
    .begin_render_pass = scegfx_command_buffer_vulkan_begin_render_pass,
    .end_render_pass = scegfx_command_buffer_vulkan_end_render_pass,
    .bind_pipeline = scegfx_command_buffer_vulkan_bind_pipeline,
  };

#endif // SCEGFX_COMMAND_BUFFER_VULKAN_H

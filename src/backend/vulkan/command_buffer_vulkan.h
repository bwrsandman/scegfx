//
// Created by sandy on 20/04/19.
//

#ifndef SCEGFX_COMMAND_BUFFER_VULKAN_H
#define SCEGFX_COMMAND_BUFFER_VULKAN_H

#include <scegfx/command_buffer.h>

#include <vulkan/vulkan.h>

enum
{
  SCEGFX_VULKAN_MAX_BARRIER_COUNT = 0x10,
};

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
void
scegfx_command_buffer_vulkan_bind_descriptor_set(
  scegfx_command_buffer_t* this,
  scegfx_pipeline_type_t bind_point,
  const scegfx_pipeline_layout_t* layout,
  const scegfx_descriptor_set_t* descriptor_set);

void
scegfx_command_buffer_vulkan_bind_vertex_buffer(scegfx_command_buffer_t* super,
                                                const scegfx_buffer_t* buffer,
                                                scegfx_device_size_t offset);
void
scegfx_command_buffer_vulkan_bind_index_buffer(
  scegfx_command_buffer_t* this,
  const scegfx_buffer_t* buffer,
  const scegfx_device_size_t offset,
  scegfx_index_type_t index_type);

void
scegfx_command_buffer_vulkan_draw(scegfx_command_buffer_t* this,
                                  uint32_t vertex_count,
                                  uint32_t instance_count,
                                  uint32_t first_vertex,
                                  uint32_t first_instance);
void
scegfx_command_buffer_vulkan_draw_indexed(scegfx_command_buffer_t* this,
                                          uint32_t index_count,
                                          uint32_t instance_count,
                                          uint32_t first_index,
                                          int32_t vertex_offset,
                                          uint32_t first_instance);

void
scegfx_command_buffer_vulkan_copy_buffer(scegfx_command_buffer_t* this,
                                         const scegfx_buffer_t* src_buffer,
                                         const scegfx_buffer_t* dst_buffer,
                                         const scegfx_buffer_copy_t* region);
void
scegfx_command_buffer_vulkan_copy_image(scegfx_command_buffer_t* this,
                                        const scegfx_image_t* src_image,
                                        scegfx_image_layout_t src_image_layout,
                                        const scegfx_image_t* dst_image,
                                        scegfx_image_layout_t dst_image_layout,
                                        const scegfx_image_copy_t* region);
void
scegfx_command_buffer_vulkan_blit_image(scegfx_command_buffer_t* this,
                                        const scegfx_image_t* src_image,
                                        scegfx_image_layout_t src_image_layout,
                                        const scegfx_image_t* dst_image,
                                        scegfx_image_layout_t dst_image_layout,
                                        const scegfx_image_blit_t* region);
void
scegfx_command_buffer_vulkan_copy_buffer_to_image(
  scegfx_command_buffer_t* this,
  const scegfx_buffer_t* buffer,
  const scegfx_image_t* image,
  scegfx_image_layout_t image_layout,
  const scegfx_buffer_image_copy_t* region);
void
scegfx_command_buffer_vulkan_copy_image_to_buffer(
  scegfx_command_buffer_t* this,
  const scegfx_image_t* image,
  scegfx_image_layout_t image_layout,
  const scegfx_buffer_t* buffer,
  const scegfx_buffer_image_copy_t* region);

void
scegfx_command_buffer_vulkan_pipeline_barrier(
  scegfx_command_buffer_t* this,
  scegfx_pipeline_stage_t src_stage_mask,
  scegfx_pipeline_stage_t dst_stage_mask,
  uint32_t image_memory_barrier_count,
  const scegfx_image_memory_barrier_t* image_memory_barriers);

void
scegfx_command_buffer_vulkan_debug_marker_begin(
  scegfx_command_buffer_t* this,
  const scegfx_debug_marker_info_t* info);
void
scegfx_command_buffer_vulkan_debug_marker_end(scegfx_command_buffer_t* this);

const static scegfx_command_buffer_api_vtable_t
  scegfx_command_buffer_api_vtable_vulkan = {
    .initialize = scegfx_command_buffer_vulkan_initialize,
    .terminate = scegfx_command_buffer_vulkan_terminate,
    .begin = scegfx_command_buffer_vulkan_begin,
    .end = scegfx_command_buffer_vulkan_end,
    .begin_render_pass = scegfx_command_buffer_vulkan_begin_render_pass,
    .end_render_pass = scegfx_command_buffer_vulkan_end_render_pass,
    .bind_pipeline = scegfx_command_buffer_vulkan_bind_pipeline,
    .bind_descriptor_set = scegfx_command_buffer_vulkan_bind_descriptor_set,
    .bind_vertex_buffer = scegfx_command_buffer_vulkan_bind_vertex_buffer,
    .bind_index_buffer = scegfx_command_buffer_vulkan_bind_index_buffer,
    .draw = scegfx_command_buffer_vulkan_draw,
    .draw_indexed = scegfx_command_buffer_vulkan_draw_indexed,
    .copy_buffer = scegfx_command_buffer_vulkan_copy_buffer,
    .copy_image = scegfx_command_buffer_vulkan_copy_image,
    .blit_image = scegfx_command_buffer_vulkan_blit_image,
    .copy_buffer_to_image = scegfx_command_buffer_vulkan_copy_buffer_to_image,
    .copy_image_to_buffer = scegfx_command_buffer_vulkan_copy_image_to_buffer,
    .pipeline_barrier = scegfx_command_buffer_vulkan_pipeline_barrier,
    .debug_marker_begin = scegfx_command_buffer_vulkan_debug_marker_begin,
    .debug_marker_end = scegfx_command_buffer_vulkan_debug_marker_end,
  };

#endif // SCEGFX_COMMAND_BUFFER_VULKAN_H

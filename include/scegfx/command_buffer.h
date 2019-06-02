//
// Created by sandy on 20/04/19.
//

#ifndef SCEGFX_COMMAND_BUFFER_H
#define SCEGFX_COMMAND_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "pipeline.h"

typedef struct scegfx_buffer_t scegfx_buffer_t;
typedef struct scegfx_command_buffer_t scegfx_command_buffer_t;
typedef struct scegfx_buffer_t scegfx_buffer_t;
typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_framebuffer_t scegfx_framebuffer_t;
typedef struct scegfx_image_t scegfx_image_t;
typedef struct scegfx_render_pass_t scegfx_render_pass_t;
typedef struct scegfx_descriptor_set_t scegfx_descriptor_set_t;

typedef enum scegfx_access_t
{
  scegfx_access_indirect_command_read = 0x00000001,
  scegfx_access_index_read = 0x00000002,
  scegfx_access_vertex_attribute_read = 0x00000004,
  scegfx_access_uniform_read = 0x00000008,
  scegfx_access_input_attachment_read = 0x00000010,
  scegfx_access_shader_read = 0x00000020,
  scegfx_access_shader_write = 0x00000040,
  scegfx_access_color_attachment_read = 0x00000080,
  scegfx_access_color_attachment_write = 0x00000100,
  scegfx_access_depth_stencil_attachment_read = 0x00000200,
  scegfx_access_depth_stencil_attachment_write = 0x00000400,
  scegfx_access_transfer_read = 0x00000800,
  scegfx_access_transfer_write = 0x00001000,
  scegfx_access_host_read = 0x00002000,
  scegfx_access_host_write = 0x00004000,
  scegfx_access_memory_read = 0x00008000,
  scegfx_access_memory_write = 0x00010000,
  scegfx_access_transform_feedback_write = 0x02000000,
  scegfx_access_transform_feedback_counter_read = 0x04000000,
  scegfx_access_transform_feedback_counter_write = 0x08000000,
  scegfx_access_conditional_rendering_read = 0x00100000,
  scegfx_access_command_process_read = 0x00020000,
  scegfx_access_command_process_write = 0x00040000,
  scegfx_access_color_attachment_read_noncoherent = 0x00080000,
  scegfx_access_shading_rate_image_read = 0x00800000,
  scegfx_access_acceleration_structure_read = 0x00200000,
  scegfx_access_acceleration_structure_write = 0x00400000,
  scegfx_access_fragment_density_map_read = 0x01000000,
} scegfx_access_t;

typedef enum scegfx_pipeline_stage_t
{
  scegfx_pipeline_stage_top_of_pipe = 0x00000001,
  scegfx_pipeline_stage_draw_indirect = 0x00000002,
  scegfx_pipeline_stage_vertex_input = 0x00000004,
  scegfx_pipeline_stage_vertex_shader = 0x00000008,
  scegfx_pipeline_stage_tessellation_control_shader = 0x00000010,
  scegfx_pipeline_stage_tessellation_evaluation_shader = 0x00000020,
  scegfx_pipeline_stage_geometry_shader = 0x00000040,
  scegfx_pipeline_stage_fragment_shader = 0x00000080,
  scegfx_pipeline_stage_early_fragment_tests = 0x00000100,
  scegfx_pipeline_stage_late_fragment_tests = 0x00000200,
  scegfx_pipeline_stage_color_attachment_output = 0x00000400,
  scegfx_pipeline_stage_compute_shader = 0x00000800,
  scegfx_pipeline_stage_transfer = 0x00001000,
  scegfx_pipeline_stage_bottom_of_pipe = 0x00002000,
  scegfx_pipeline_stage_host = 0x00004000,
  scegfx_pipeline_stage_all_graphics = 0x00008000,
  scegfx_pipeline_stage_all_commands = 0x00010000,
  scegfx_pipeline_stage_transform_feedback = 0x01000000,
  scegfx_pipeline_stage_conditional_rendering = 0x00040000,
  scegfx_pipeline_stage_command_process = 0x00020000,
  scegfx_pipeline_stage_shading_rate_image = 0x00400000,
  scegfx_pipeline_stage_ray_tracing_shader = 0x00200000,
  scegfx_pipeline_stage_acceleration_structure_build = 0x02000000,
  scegfx_pipeline_stage_task_shader = 0x00080000,
  scegfx_pipeline_stage_mesh_shader = 0x00100000,
  scegfx_pipeline_stage_fragment_density_process = 0x00800000,
} scegfx_pipeline_stage_t;

typedef struct scegfx_render_pass_begin_info_t
{
  scegfx_rect_2d_t render_area;
  struct
  {
    float color[4];
    float depth;
  } clear_values;
  scegfx_render_pass_t* render_pass;
  scegfx_framebuffer_t* framebuffer;
} scegfx_render_pass_begin_info_t;

typedef struct scegfx_image_subresource_layers_t
{
  scegfx_image_aspect_t aspect_mask;
  uint32_t mip_level;
  uint32_t base_array_layer;
  uint32_t layer_count;
} scegfx_image_subresource_layers_t;

typedef struct scegfx_image_subresource_range_t
{
  scegfx_image_aspect_t aspect_mask;
  uint32_t base_mip_level;
  uint32_t level_count;
  uint32_t base_array_layer;
  uint32_t layer_count;
} scegfx_image_subresource_range_t;

typedef struct scegfx_image_memory_barrier_t
{
  scegfx_access_t src_access_mask;
  scegfx_access_t dst_access_mask;
  scegfx_image_layout_t old_layout;
  scegfx_image_layout_t new_layout;
  scegfx_image_t* image;
  scegfx_image_subresource_range_t subresource_range;
} scegfx_image_memory_barrier_t;

typedef struct scegfx_debug_marker_info_t
{
  const char* name;
  float color[4];
} scegfx_debug_marker_info_t;

typedef struct scegfx_buffer_copy_t
{
  scegfx_device_size_t src_offset;
  scegfx_device_size_t dst_offset;
  scegfx_device_size_t size;
} scegfx_buffer_copy_t;

typedef struct scegfx_image_copy_t
{
  scegfx_image_subresource_layers_t src_subresource;
  scegfx_offset_3d_t src_offset;
  scegfx_image_subresource_layers_t dst_subresource;
  scegfx_offset_3d_t dst_offset;
  scegfx_extent_3d_t extent;
} scegfx_image_copy_t;

typedef struct scegfx_image_blit_t
{
  scegfx_image_subresource_layers_t src_subresource;
  scegfx_offset_3d_t src_offsets[2];
  scegfx_image_subresource_layers_t dst_subresource;
  scegfx_offset_3d_t dst_offsets[2];
} scegfx_image_blit_t;

typedef struct scegfx_buffer_image_copy_t
{
  scegfx_device_size_t buffer_offset;
  uint32_t buffer_row_length;
  uint32_t buffer_image_height;
  scegfx_image_subresource_layers_t image_subresource;
  scegfx_offset_3d_t image_offset;
  scegfx_extent_3d_t image_extent;
} scegfx_buffer_image_copy_t;

typedef struct scegfx_command_buffer_api_vtable_t
{
  bool (*initialize)(scegfx_command_buffer_t* this);
  void (*terminate)(scegfx_command_buffer_t* this);

  bool (*begin)(scegfx_command_buffer_t* this, bool long_lived);
  bool (*end)(scegfx_command_buffer_t* this);

  void (*begin_render_pass)(scegfx_command_buffer_t* this,
                            const scegfx_render_pass_begin_info_t* info);
  void (*end_render_pass)(scegfx_command_buffer_t* this);

  void (*bind_pipeline)(scegfx_command_buffer_t* this,
                        scegfx_pipeline_type_t type,
                        const scegfx_pipeline_t* pipeline);

  void (*bind_descriptor_set)(scegfx_command_buffer_t* this,
                              scegfx_pipeline_type_t bind_point,
                              const scegfx_pipeline_layout_t* layout,
                              const scegfx_descriptor_set_t* descriptor_set);

  void (*bind_vertex_buffer)(scegfx_command_buffer_t* this,
                             const scegfx_buffer_t* buffer,
                             scegfx_device_size_t offset);
  void (*bind_index_buffer)(scegfx_command_buffer_t* this,
                            const scegfx_buffer_t* buffer,
                            const scegfx_device_size_t offset,
                            scegfx_index_type_t index_type);

  void (*draw)(scegfx_command_buffer_t* this,
               uint32_t vertex_count,
               uint32_t instance_count,
               uint32_t first_vertex,
               uint32_t first_instance);
  void (*draw_indexed)(scegfx_command_buffer_t* this,
                       uint32_t index_count,
                       uint32_t instance_count,
                       uint32_t first_index,
                       int32_t vertex_offset,
                       uint32_t first_instance);

  void (*copy_buffer)(scegfx_command_buffer_t* this,
                      const scegfx_buffer_t* src_buffer,
                      const scegfx_buffer_t* dst_buffer,
                      const scegfx_buffer_copy_t* region);
  void (*copy_image)(scegfx_command_buffer_t* this,
                     const scegfx_image_t* src_image,
                     scegfx_image_layout_t src_image_layout,
                     const scegfx_image_t* dst_image,
                     scegfx_image_layout_t dst_image_layout,
                     const scegfx_image_copy_t* region);
  void (*blit_image)(scegfx_command_buffer_t* this,
                     const scegfx_image_t* src_image,
                     scegfx_image_layout_t src_image_layout,
                     const scegfx_image_t* dst_image,
                     scegfx_image_layout_t dst_image_layout,
                     const scegfx_image_blit_t* region);
  void (*copy_buffer_to_image)(scegfx_command_buffer_t* this,
                               const scegfx_buffer_t* buffer,
                               const scegfx_image_t* image,
                               scegfx_image_layout_t image_layout,
                               const scegfx_buffer_image_copy_t* region);
  void (*copy_image_to_buffer)(scegfx_command_buffer_t* this,
                               const scegfx_image_t* image,
                               scegfx_image_layout_t image_layout,
                               const scegfx_buffer_t* buffer,
                               const scegfx_buffer_image_copy_t* region);

  void (*pipeline_barrier)(
    scegfx_command_buffer_t* this,
    scegfx_pipeline_stage_t src_stage_mask,
    scegfx_pipeline_stage_t dst_stage_mask,
    uint32_t image_memory_barrier_count,
    const scegfx_image_memory_barrier_t* image_memory_barriers);

  void (*debug_marker_begin)(scegfx_command_buffer_t* this,
                             const scegfx_debug_marker_info_t* info);
  void (*debug_marker_end)(scegfx_command_buffer_t* this);
} scegfx_command_buffer_api_vtable_t;

typedef struct scegfx_command_buffer_t
{
  bool initialized;
  const scegfx_command_buffer_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_command_buffer_t;

#endif // SCEGFX_COMMAND_BUFFER_H

//
// Created by sandy on 22/04/19.
//

#ifndef SCEGFX_COMMON_H
#define SCEGFX_COMMON_H

#include <stdint.h>

typedef uint64_t scegfx_device_size_t;

typedef enum scegfx_format_t
{
  scegfx_format_r8g8b8a8_unorm = 37,
  scegfx_format_r32g32_sfloat = 103,
  scegfx_format_r32g32b32_sfloat = 106,
  scegfx_format_r32g32b32a32_sfloat = 109,
  scegfx_format_d32_sfloat = 126,
  scegfx_format_s8_uint = 127,
  scegfx_format_d16_unorm_s8_uint = 128,
  scegfx_format_d24_unorm_s8_uint = 129,
  scegfx_format_d32_unorm_s8_uint = 130,
} scegfx_format_t;

typedef enum scegfx_index_type_t
{
  scegfx_index_type_u16,
  scegfx_index_type_u32,
} scegfx_index_type_t;

typedef enum scegfx_memory_properties_t
{
  scegfx_memory_properties_device_local = 0x0001u,
  scegfx_memory_properties_host_visible = 0x0002u,
  scegfx_memory_properties_host_coherent = 0x0004u,
  scegfx_memory_properties_host_cached = 0x0008u,
  scegfx_memory_properties_lazily_allocated = 0x0010u,
  scegfx_memory_properties_protected = 0x0020u,
} scegfx_memory_properties_t;

typedef enum scegfx_stage_type_t
{
  scegfx_stage_type_vertex = 0x00000001,
  scegfx_stage_type_tessellation_control = 0x00000002,
  scegfx_stage_type_tessellation_evaluation = 0x00000004,
  scegfx_stage_type_geometry = 0x00000008,
  scegfx_stage_type_fragment = 0x00000010,
  scegfx_stage_type_compute = 0x00000020,
  scegfx_stage_type_raygen = 0x00000100,
  scegfx_stage_type_any_hit = 0x00000200,
  scegfx_stage_type_closest_hit = 0x00000400,
  scegfx_stage_type_miss = 0x00000800,
  scegfx_stage_type_intersection = 0x00001000,
} scegfx_stage_type_t;

typedef enum scegfx_image_aspect_t
{
  scegfx_image_aspect_color = 0x0001,
  scegfx_image_aspect_depth = 0x0002,
  scegfx_image_aspect_stencil = 0x0004,
} scegfx_image_aspect_t;

typedef enum scegfx_image_layout_t
{
  scegfx_image_layout_undefined = 0,
  scegfx_image_layout_general = 1,
  scegfx_image_layout_color_attachment_optimal = 2,
  scegfx_image_layout_depth_stencil_attachment_optimal = 3,
  scegfx_image_layout_depth_stencil_read_only_optimal = 4,
  scegfx_image_layout_shader_read_only_optimal = 5,
  scegfx_image_layout_transfer_src_optimal = 6,
  scegfx_image_layout_transfer_dst_optimal = 7,
  scegfx_image_layout_preinitialized = 8,
} scegfx_image_layout_t;

typedef struct scegfx_offset_2d_t
{
  int32_t x;
  int32_t y;
} scegfx_offset_2d_t;

typedef struct scegfx_offset_3d_t
{
  int32_t x;
  int32_t y;
  int32_t z;
} scegfx_offset_3d_t;

typedef struct scegfx_extent_2d_t
{
  uint32_t width;
  uint32_t height;
} scegfx_extent_2d_t;

typedef struct scegfx_extent_3d_t
{
  uint32_t width;
  uint32_t height;
  uint32_t depth;
} scegfx_extent_3d_t;

typedef struct scegfx_rect_2d_t
{
  scegfx_offset_2d_t offset;
  scegfx_extent_2d_t extent;
} scegfx_rect_2d_t;

#endif // SCEGFX_COMMON_H

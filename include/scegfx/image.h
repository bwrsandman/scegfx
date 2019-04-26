#ifndef SCEGFX_IMAGE_H
#define SCEGFX_IMAGE_H

#include <stdbool.h>

#include "common.h"

typedef struct scegfx_image_t scegfx_image_t;
typedef struct scegfx_context_t scegfx_context_t;

typedef enum scegfx_image_type_t
{
  scegfx_image_type_1d = 0,
  scegfx_image_type_2d = 1,
  scegfx_image_type_3d = 2,
} scegfx_image_type_t;

typedef enum scegfx_image_usage_t
{
  scegfx_image_usage_transfer_src = 0x0001,
  scegfx_image_usage_transfer_dst = 0x0002,
  scegfx_image_usage_sampled = 0x0004,
  scegfx_image_usage_storage = 0x0008,
  scegfx_image_usage_color_attachment = 0x0010,
  scegfx_image_usage_depth_stencil_attachment = 0x0020,
  scegfx_image_usage_transient_attachment = 0x0040,
  scegfx_image_usage_input_attachment = 0x0080,
  scegfx_image_usage_shading_rate_image = 0x0100,
  scegfx_image_usage_fragment_density_map = 0x0200,
} scegfx_image_usage_t;

typedef enum scegfx_image_tiling_t
{
  scegfx_image_tiling_optimal = 0,
  scegfx_image_tiling_linear = 1,
  scegfx_image_tiling_drm_format_modifier = 1000158000,
} scegfx_image_tiling_t;

typedef struct scegfx_image_create_info_t
{
  scegfx_image_type_t image_type;
  scegfx_format_t format;
  scegfx_extent_3d_t extent;
  scegfx_image_tiling_t tiling;
  scegfx_image_usage_t usage;
  scegfx_image_layout_t initial_layout;
} scegfx_image_create_info_t;

typedef struct scegfx_image_api_vtable_t
{
  bool (*initialize)(scegfx_image_t* this,
                     const scegfx_image_create_info_t* info);
  void (*terminate)(scegfx_image_t* this);
} scegfx_image_api_vtable_t;

typedef struct scegfx_image_t
{
  bool initialized;
  const scegfx_image_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
  scegfx_image_type_t type;
  scegfx_format_t format;
  scegfx_extent_3d_t extent;
} scegfx_image_t;

#endif // SCEGFX_IMAGE_H

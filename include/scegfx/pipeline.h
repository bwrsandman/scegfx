//
// Created by sandy on 06/05/19.
//

#ifndef SCEGFX_PIPELINE_H
#define SCEGFX_PIPELINE_H

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

typedef struct scegfx_pipeline_t scegfx_pipeline_t;
typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_pipeline_layout_t scegfx_pipeline_layout_t;
typedef struct scegfx_render_pass_t scegfx_render_pass_t;
typedef struct scegfx_shader_module_t scegfx_shader_module_t;
typedef struct scegfx_shader_stage_t scegfx_shader_stage_t;

typedef enum scegfx_pipeline_type_t
{
  scegfx_pipeline_type_graphics,
  scegfx_pipeline_type_compute,
  scegfx_pipeline_type_ray_tracing,
} scegfx_pipeline_type_t;

typedef enum scegfx_raster_state_cull_mode_t
{
  scegfx_raster_state_cull_mode_none = 0x00,
  scegfx_raster_state_cull_mode_front = 0x01,
  scegfx_raster_state_cull_mode_back = 0x02,
  scegfx_raster_state_cull_mode_front_and_back = 0x03,
} scegfx_raster_state_cull_mode_t;

typedef enum scegfx_raster_state_front_face_t
{
  scegfx_raster_state_front_face_counter_clockwise = 0,
  scegfx_raster_state_front_face_clockwise = 1,
} scegfx_raster_state_front_face_t;

typedef struct scegfx_vertex_input_binding_description_t
{
  uint32_t stride;
} scegfx_vertex_input_binding_description_t;

typedef struct scegfx_vertex_input_attribute_description_t
{
  uint32_t location;
  scegfx_format_t format;
  uint32_t offset;
} scegfx_vertex_input_attribute_description_t;

typedef struct scegfx_pipeline_create_info_t
{
  scegfx_pipeline_type_t type;
  scegfx_pipeline_layout_t* layout;
  union
  {
    struct
    {
      union
      {
        struct
        {
          const scegfx_shader_module_t* vertex;
          const scegfx_shader_module_t* tessellation_control;
          const scegfx_shader_module_t* tessellation_evaluation;
          const scegfx_shader_module_t* geometry;
          const scegfx_shader_module_t* fragment;
        };
        const scegfx_shader_module_t* _all_stages[5];
      } stages;
      struct
      {
        scegfx_raster_state_cull_mode_t cull_mode;
        scegfx_raster_state_front_face_t front_face;
        float line_width;
      } raster_state;
      const scegfx_render_pass_t* render_pass;
      struct
      {
        scegfx_vertex_input_binding_description_t binding_description;
        uint32_t attribute_description_count;
        const scegfx_vertex_input_attribute_description_t*
          attribute_descriptions;
      } vertex_input_state;
    } graphics;
    struct
    {
      const scegfx_shader_module_t* stage;
    } compute;
    struct
    {
      union
      {
        struct
        {
          const scegfx_shader_module_t* raygen;
          const scegfx_shader_module_t* any_hit;
          const scegfx_shader_module_t* closest_hit;
          const scegfx_shader_module_t* miss;
          const scegfx_shader_module_t* intersection;
        };
        const scegfx_shader_module_t* _all_stages[5];
      } stages;
      //      group_count
      //      groups
      uint32_t max_recursion_depth;
    } ray_tracing;
  };
} scegfx_pipeline_create_info_t;

typedef struct scegfx_pipeline_api_vtable_t
{
  bool (*initialize)(scegfx_pipeline_t* this,
                     const scegfx_pipeline_create_info_t* info);
  void (*terminate)(scegfx_pipeline_t* this);
} scegfx_pipeline_api_vtable_t;

typedef struct scegfx_pipeline_t
{
  bool initialized;
  const scegfx_pipeline_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_pipeline_t;

#endif // SCEGFX_PIPELINE_H

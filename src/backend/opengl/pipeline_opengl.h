//
// Created by sandy on 10/05/19.
//

#ifndef SCEGFX_PIPELINE_OPENGL_H
#define SCEGFX_PIPELINE_OPENGL_H

#include <scegfx/pipeline.h>

enum
{
  SCEGFX_OPENGL_MAX_VERTEX_ATTRIBUTE_COUNT = 2,
};

typedef struct scegfx_pipeline_opengl_t
{
  scegfx_pipeline_t super;
  scegfx_pipeline_type_t type;
  union
  {
    struct
    {
      uint32_t topology;
      scegfx_vertex_input_binding_description_t binding_desc;
      uint32_t attribute_desc_count;
      scegfx_vertex_input_attribute_description_t
        attribute_descs[SCEGFX_OPENGL_MAX_VERTEX_ATTRIBUTE_COUNT];
      uint32_t front_face;
      uint32_t cull_face;
      float line_width;
    } graphics;
  };
  uint32_t program;
} scegfx_pipeline_opengl_t;

bool
scegfx_pipeline_opengl_initialize(scegfx_pipeline_t* this,
                                  const scegfx_pipeline_create_info_t* info);
void
scegfx_pipeline_opengl_terminate(scegfx_pipeline_t* this);

const static scegfx_pipeline_api_vtable_t scegfx_pipeline_api_vtable_opengl = {
  .initialize = scegfx_pipeline_opengl_initialize,
  .terminate = scegfx_pipeline_opengl_terminate,
};

#endif // SCEGFX_PIPELINE_OPENGL_H

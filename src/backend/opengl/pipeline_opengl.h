//
// Created by sandy on 10/05/19.
//

#ifndef SCEGFX_PIPELINE_OPENGL_H
#define SCEGFX_PIPELINE_OPENGL_H

#include <scegfx/pipeline.h>

typedef struct scegfx_pipeline_opengl_t
{
  scegfx_pipeline_t super;
  scegfx_pipeline_type_t type;
  union
  {
    struct
    {
      uint32_t topology;
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

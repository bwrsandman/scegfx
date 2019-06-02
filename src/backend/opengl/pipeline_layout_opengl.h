//
// Created by sandy on 10/05/19.
//

#ifndef SCEGFX_PIPELINE_LAYOUT_OPENGL_H
#define SCEGFX_PIPELINE_LAYOUT_OPENGL_H

#include <scegfx/pipeline_layout.h>

typedef struct scegfx_pipeline_layout_opengl_t
{
  scegfx_pipeline_layout_t super;
} scegfx_pipeline_layout_opengl_t;

bool
scegfx_pipeline_layout_opengl_initialize(
  scegfx_pipeline_layout_t* this,
  const scegfx_pipeline_layout_create_info_t* info);
void
scegfx_pipeline_layout_opengl_terminate(scegfx_pipeline_layout_t* this);

const static scegfx_pipeline_layout_api_vtable_t
  scegfx_pipeline_layout_api_vtable_opengl = {
    .initialize = scegfx_pipeline_layout_opengl_initialize,
    .terminate = scegfx_pipeline_layout_opengl_terminate,
  };

#endif // SCEGFX_PIPELINE_LAYOUT_OPENGL_H

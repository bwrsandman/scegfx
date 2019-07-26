//
// Created by sandy on 26/07/19.
//

#ifndef SCEGFX_SAMPLER_OPENGL_H
#define SCEGFX_SAMPLER_OPENGL_H

#include <scegfx/sampler.h>

#include <stdint.h>

typedef struct scegfx_sampler_opengl_t
{
  scegfx_sampler_t super;
  int32_t mag_filter;
  int32_t min_filter;
  int32_t texture_wrap_s;
  int32_t texture_wrap_t;
  int32_t texture_wrap_r;
} scegfx_sampler_opengl_t;

bool
scegfx_sampler_opengl_initialize(scegfx_sampler_t* this,
                                 const scegfx_sampler_create_info_t* info);
void
scegfx_sampler_opengl_terminate(scegfx_sampler_t* this);

const static scegfx_sampler_api_vtable_t scegfx_sampler_api_vtable_opengl = {
  .initialize = scegfx_sampler_opengl_initialize,
  .terminate = scegfx_sampler_opengl_terminate,
};

#endif // SCEGFX_SAMPLER_OPENGL_H

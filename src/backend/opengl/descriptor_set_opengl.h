//
// Created by sandy on 09/06/19.
//

#ifndef SCEGFX_DESCRIPTOR_SET_OPENGL_H
#define SCEGFX_DESCRIPTOR_SET_OPENGL_H

#include <scegfx/descriptor_set.h>

#include "descriptor_set_layout_opengl.h"

typedef struct scegfx_descriptor_t
{
  uint32_t index;
  scegfx_descriptor_type_t type;
  union
  {
    struct
    {
      struct
      {
        int32_t mag_filter;
        int32_t min_filter;
        int32_t texture_wrap_s;
        int32_t texture_wrap_t;
        int32_t texture_wrap_r;
      } sampler;
      struct
      {
        uint32_t type;
        uint32_t handle;
      } image;
    } sampled_image;
    struct
    {
      uint32_t handle;
      scegfx_device_size_t offset;
      scegfx_device_size_t range;
    } buffer;
  };
} scegfx_descriptor_t;

typedef struct scegfx_descriptor_set_opengl_t
{
  scegfx_descriptor_set_t super;
  uint32_t descriptor_count;
  scegfx_descriptor_t descriptors[SCEGFX_OPENGL_MAX_BINDING_COUNT];
} scegfx_descriptor_set_opengl_t;

#endif // SCEGFX_DESCRIPTOR_SET_OPENGL_H

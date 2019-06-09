#ifndef SCEGFX_IMAGE_VIEW_H
#define SCEGFX_IMAGE_VIEW_H

#include <stdbool.h>

#include "common.h"

typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_image_view_t scegfx_image_view_t;
typedef struct scegfx_image_t scegfx_image_t;

typedef struct scegfx_image_view_api_vtable_t
{
  bool (*initialize)(scegfx_image_view_t* this,
                     const scegfx_image_t* image,
                     scegfx_image_aspect_t aspect);
  void (*terminate)(scegfx_image_view_t* this);
} scegfx_image_view_api_vtable_t;

typedef struct scegfx_image_view_t
{
  bool initialized;
  const scegfx_image_view_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
  scegfx_extent_3d_t extent;
} scegfx_image_view_t;

#endif // SCEGFX_IMAGE_VIEW_H

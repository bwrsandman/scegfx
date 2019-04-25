//
// Created by sandy on 24/04/19.
//

#ifndef SCEGFX_SWAPCHAIN_H
#define SCEGFX_SWAPCHAIN_H

#include <stdbool.h>

#include "allocator.h"
#include "common.h"

typedef struct SDL_Window SDL_Window;
typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_swapchain_t scegfx_swapchain_t;

typedef struct scegfx_swapchain_api_vtable_t
{
  bool (*initialize)(scegfx_swapchain_t* this);
  void (*terminate)(scegfx_swapchain_t* this);
} scegfx_swapchain_api_vtable_t;

typedef struct scegfx_swapchain_t
{
  bool initialized;
  const scegfx_swapchain_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
  bool flip_y;
  scegfx_extent_2d_t extent;
  uint32_t image_count;
} scegfx_swapchain_t;

#endif // SCEGFX_SWAPCHAIN_H

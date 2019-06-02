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
typedef struct scegfx_descriptor_set_t scegfx_descriptor_set_t;
typedef struct scegfx_image_t scegfx_image_t;
typedef struct scegfx_swapchain_t scegfx_swapchain_t;

typedef struct scegfx_swapchain_api_vtable_t
{
  bool (*initialize)(scegfx_swapchain_t* this);
  void (*terminate)(scegfx_swapchain_t* this);

  scegfx_image_t* (*get_image)(scegfx_swapchain_t* this, uint32_t index);
  bool (*acquire_next_image)(scegfx_swapchain_t* this,
                             uint64_t timeout,
                             scegfx_semaphore_t* semaphore,
                             scegfx_fence_t* fence,
                             uint32_t* image_index);

  scegfx_descriptor_set_t* (*allocate_descriptor_set)(
    scegfx_swapchain_t* this,
    const scegfx_descriptor_set_layout_t* layout,
    scegfx_allocator_t* allocator);
  void (*free_descriptor_set)(scegfx_swapchain_t* this,
                              scegfx_descriptor_set_t* descriptor_set,
                              scegfx_allocator_t* allocator);
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

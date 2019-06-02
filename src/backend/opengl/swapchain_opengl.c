//
// Created by sandy on 24/04/19.
//

#include "swapchain_opengl.h"

#include <assert.h>

#include "context_opengl.h"
#include "descriptor_set_layout_opengl.h"
#include "descriptor_set_opengl.h"
#include "fence_opengl.h"
#include "image_opengl.h"
#include "semaphore_opengl.h"

bool
scegfx_swapchain_opengl_initialize(scegfx_swapchain_t* super)
{
  assert(!super->initialized);
  scegfx_swapchain_opengl_t* this = (scegfx_swapchain_opengl_t*)super;
  const scegfx_context_opengl_t* context =
    (const scegfx_context_opengl_t*)super->context;
  int width, height;

  this->super.flip_y = true;
  SDL_GL_GetDrawableSize(context->super.window_handle, &width, &height);
  this->super.extent.width = width;
  this->super.extent.height = height;

  scegfx_image_opengl_t* image = &this->image;
  image->is_default_framebuffer = true;
  image->super.type = scegfx_image_type_2d;
  image->super.format = scegfx_format_r32g32b32a32_sfloat;
  image->format = /*GL_RGBA32F*/ 0x8814;
  image->super.extent.width = this->super.extent.width;
  image->super.extent.height = this->super.extent.height;
  image->super.extent.depth = 1;
  image->super.initialized = true;

  this->super.image_count = 1;

  this->super.initialized = true;
  return true;
}

void
scegfx_swapchain_opengl_terminate(scegfx_swapchain_t* super)
{
  assert(super->initialized);
  super->initialized = false;
}

scegfx_image_t*
scegfx_swapchain_opengl_get_image(scegfx_swapchain_t* super, uint32_t index)
{
  assert(super->initialized);
  assert(index == 0);

  scegfx_swapchain_opengl_t* this = (scegfx_swapchain_opengl_t*)super;

  return (scegfx_image_t*)&this->image;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
bool
scegfx_swapchain_opengl_acquire_next_image(scegfx_swapchain_t* super,
                                           uint64_t timeout,
                                           scegfx_semaphore_t* semaphore,
                                           scegfx_fence_t* fence,
                                           uint32_t* image_index)
{
  assert(super->initialized);
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;
  scegfx_semaphore_opengl_t* semaphore_gl =
    (scegfx_semaphore_opengl_t*)semaphore;
  scegfx_fence_opengl_t* fence_gl = (scegfx_fence_opengl_t*)fence;

  // Unblock GPU wait in command queue
  assert(scegfx_semaphore_opengl_signal(semaphore_gl));

  // Insert CPU fence sync in the command queue
  assert(scegfx_fence_opengl_signal(fence_gl));

  *image_index = 0;
  return true;
}
#pragma clang diagnostic pop

scegfx_descriptor_set_t*
scegfx_swapchain_opengl_allocate_descriptor_set(
  scegfx_swapchain_t* super,
  const scegfx_descriptor_set_layout_t* layout,
  scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  assert(layout->initialized);
  scegfx_descriptor_set_layout_opengl_t* gl_layout =
    (scegfx_descriptor_set_layout_opengl_t*)layout;

  scegfx_descriptor_set_opengl_t* descriptor_set = NULL;
  if (allocator == NULL)
    descriptor_set = malloc(sizeof(scegfx_descriptor_set_opengl_t));
  else
    descriptor_set = allocator->allocator_callback(
      NULL, sizeof(scegfx_descriptor_set_opengl_t), allocator->user_data);
  memset(descriptor_set, 0, sizeof(scegfx_descriptor_set_opengl_t));

  assert(gl_layout->binding_count <= SCEGFX_OPENGL_MAX_BINDING_COUNT);

  descriptor_set->descriptor_count = gl_layout->binding_count;
  for (uint32_t i = 0; i < gl_layout->binding_count; ++i) {
    descriptor_set->descriptors[i].index = gl_layout->bindings[i];
  }

  descriptor_set->super.initialized = true;
  return (scegfx_descriptor_set_t*)descriptor_set;
}

void
scegfx_swapchain_opengl_free_descriptor_set(
  scegfx_swapchain_t* super,
  scegfx_descriptor_set_t* descriptor_set,
  scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  if (!descriptor_set->initialized) {
    return;
  }

  descriptor_set->initialized = false;

  if (allocator == NULL) {
    free(descriptor_set);
  } else {
    allocator->allocator_callback(descriptor_set, 0, allocator->user_data);
  }
}

//
// Created by sandy on 21/03/19.
//

#ifndef SCEGFX_CONTEXT_OPENGL_H
#define SCEGFX_CONTEXT_OPENGL_H

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL_video.h>
#include <spirv_cross_c.h>

#include <scegfx/context.h>

typedef struct sce_private_window_t sce_private_window_t;

enum
{
  SCEGFX_OPENGL_MAX_EXTENSION_STRING_SIZE = 64
};
enum
{
  SCEGFX_OPENGL_MAX_EXTENSION_COUNT = 512
};

typedef void (*PFNGLSPECIALIZESHADERPROC)(uint32_t shader,
                                          const char* pEntryPoint,
                                          uint32_t numSpecializationConstants,
                                          const uint32_t* pConstantIndex,
                                          const uint32_t* pConstantValue);

typedef struct scegfx_context_opengl_t
{
  scegfx_context_t super;
  spvc_context spvc;
#if !defined(EMSCRIPTEN)
  struct
  {
    PFNGLSPECIALIZESHADERPROC SpecializeShader;
  } functions;
#endif
  uint32_t extension_count;
  char extension_names[SCEGFX_OPENGL_MAX_EXTENSION_COUNT]
                      [SCEGFX_OPENGL_MAX_EXTENSION_STRING_SIZE];
  uint64_t max_client_wait_timeout;
  uint64_t max_server_wait_timeout;
} scegfx_context_opengl_t;

bool
scegfx_context_opengl_initialize(scegfx_context_t* this);
void
scegfx_context_opengl_terminate(scegfx_context_t* this);

uint32_t
scegfx_context_opengl_get_memory_type(scegfx_context_t* this,
                                      uint32_t type_bits,
                                      scegfx_memory_properties_t properties);
scegfx_device_memory_t*
scegfx_context_opengl_allocate_memory(
  scegfx_context_t* this,
  const scegfx_device_memory_allocate_info_t* info,
  scegfx_allocator_t* allocator);
void
scegfx_context_opengl_free_memory(scegfx_context_t* this,
                                  scegfx_device_memory_t* memory,
                                  scegfx_allocator_t* allocator);
bool
scegfx_context_opengl_map_memory(scegfx_context_t* this,
                                 scegfx_device_memory_t* memory,
                                 scegfx_device_size_t offset,
                                 scegfx_device_size_t size,
                                 void** data);
void
scegfx_context_opengl_unmap_memory(scegfx_context_t* this,
                                   scegfx_device_memory_t* memory);

bool
scegfx_context_opengl_flush_mapped_memory_ranges(
  scegfx_context_t* this,
  uint32_t memory_range_count,
  const scegfx_mapped_device_memory_range_t* memory_ranges);

scegfx_buffer_t*
scegfx_context_opengl_create_buffer(scegfx_context_t* this,
                                    scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_buffer(scegfx_context_t* this,
                                     scegfx_buffer_t* buffer,
                                     scegfx_allocator_t* allocator);
void
scegfx_context_opengl_get_buffer_memory_requirements(
  scegfx_context_t* this,
  const scegfx_buffer_t* buffer,
  scegfx_device_memory_requirements_t* memory_requirements);

scegfx_image_t*
scegfx_context_opengl_create_image(scegfx_context_t* this,
                                   scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_image(scegfx_context_t* this,
                                    scegfx_image_t* image,
                                    scegfx_allocator_t* allocator);
void
scegfx_context_opengl_get_image_memory_requirements(
  const scegfx_context_t* this,
  const scegfx_image_t* image,
  scegfx_device_memory_requirements_t* memory_requirements);

bool
scegfx_context_opengl_bind_buffer_memory(scegfx_context_t* this,
                                         scegfx_buffer_t* buffer,
                                         scegfx_device_memory_t* memory,
                                         scegfx_device_size_t memory_offset);
bool
scegfx_context_opengl_bind_image_memory(scegfx_context_t* this,
                                        scegfx_image_t* image,
                                        scegfx_device_memory_t* memory,
                                        scegfx_device_size_t memory_offset);

scegfx_swapchain_t*
scegfx_context_opengl_create_swapchain(scegfx_context_t* this,
                                       scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_swapchain(scegfx_context_t* this,
                                        scegfx_swapchain_t* swapchain,
                                        scegfx_allocator_t* allocator);

scegfx_fence_t*
scegfx_context_opengl_create_fence(scegfx_context_t* this,
                                   scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_fence(scegfx_context_t* this,
                                    scegfx_fence_t* fence,
                                    scegfx_allocator_t* allocator);

scegfx_semaphore_t*
scegfx_context_opengl_create_semaphore(scegfx_context_t* this,
                                       scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_semaphore(scegfx_context_t* this,
                                        scegfx_semaphore_t* semaphore,
                                        scegfx_allocator_t* allocator);

scegfx_image_view_t*
scegfx_context_opengl_create_image_view(scegfx_context_t* super,
                                        scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_image_view(scegfx_context_t* this,
                                         scegfx_image_view_t* view,
                                         scegfx_allocator_t* allocator);
scegfx_sampler_t*
scegfx_context_opengl_create_sampler(scegfx_context_t* super,
                                     scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_sampler(scegfx_context_t* this,
                                      scegfx_sampler_t* sampler,
                                      scegfx_allocator_t* allocator);

scegfx_render_pass_t*
scegfx_context_opengl_create_render_pass(scegfx_context_t* this,
                                         scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_render_pass(scegfx_context_t* this,
                                          scegfx_render_pass_t* render_pass,
                                          scegfx_allocator_t* allocator);

scegfx_framebuffer_t*
scegfx_context_opengl_create_framebuffer(scegfx_context_t* this,
                                         scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_framebuffer(scegfx_context_t* this,
                                          scegfx_framebuffer_t* framebuffer,
                                          scegfx_allocator_t* allocator);

scegfx_command_buffer_t*
scegfx_context_opengl_create_command_buffer(scegfx_context_t* super,
                                            scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_command_buffer(scegfx_context_t* super,
                                             scegfx_command_buffer_t* queue,
                                             scegfx_allocator_t* allocator);

scegfx_shader_module_t*
scegfx_context_opengl_create_shader_module(scegfx_context_t* this,
                                           scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_shader_module(
  scegfx_context_t* this,
  scegfx_shader_module_t* shader_module,
  scegfx_allocator_t* allocator);

scegfx_pipeline_layout_t*
scegfx_context_opengl_create_pipeline_layout(scegfx_context_t* this,
                                             scegfx_allocator_t* allocator);
void
scegfx_context_opengl_destroy_pipeline_layout(scegfx_context_t* this,
                                              scegfx_pipeline_layout_t* layout,
                                              scegfx_allocator_t* allocator);

bool
scegfx_context_opengl_make_current(scegfx_context_t* this);
bool
scegfx_context_opengl_submit_to_queue(scegfx_context_t* this,
                                      scegfx_submit_info_t* info,
                                      scegfx_fence_t* fence);
bool
scegfx_context_opengl_present(scegfx_context_t* this,
                              scegfx_present_info_t* info);
bool
scegfx_context_opengl_wait_idle(scegfx_context_t* this);

static const scegfx_context_api_vtable_t scegfx_context_api_vtable_opengl = {
  .initialize = scegfx_context_opengl_initialize,
  .terminate = scegfx_context_opengl_terminate,
  .get_memory_type = scegfx_context_opengl_get_memory_type,
  .allocate_memory = scegfx_context_opengl_allocate_memory,
  .free_memory = scegfx_context_opengl_free_memory,
  .map_memory = scegfx_context_opengl_map_memory,
  .unmap_memory = scegfx_context_opengl_unmap_memory,
  .flush_mapped_memory_ranges =
    scegfx_context_opengl_flush_mapped_memory_ranges,
  .create_buffer = scegfx_context_opengl_create_buffer,
  .destroy_buffer = scegfx_context_opengl_destroy_buffer,
  .get_buffer_memory_requirements =
    scegfx_context_opengl_get_buffer_memory_requirements,
  .create_image = scegfx_context_opengl_create_image,
  .destroy_image = scegfx_context_opengl_destroy_image,
  .get_image_memory_requirements =
    scegfx_context_opengl_get_image_memory_requirements,
  .bind_buffer_memory = scegfx_context_opengl_bind_buffer_memory,
  .bind_image_memory = scegfx_context_opengl_bind_image_memory,
  .create_swapchain = scegfx_context_opengl_create_swapchain,
  .destroy_swapchain = scegfx_context_opengl_destroy_swapchain,
  .create_fence = scegfx_context_opengl_create_fence,
  .destroy_fence = scegfx_context_opengl_destroy_fence,
  .create_semaphore = scegfx_context_opengl_create_semaphore,
  .destroy_semaphore = scegfx_context_opengl_destroy_semaphore,
  .create_image_view = scegfx_context_opengl_create_image_view,
  .destroy_image_view = scegfx_context_opengl_destroy_image_view,
  .create_sampler = scegfx_context_opengl_create_sampler,
  .destroy_sampler = scegfx_context_opengl_destroy_sampler,
  .create_render_pass = scegfx_context_opengl_create_render_pass,
  .destroy_render_pass = scegfx_context_opengl_destroy_render_pass,
  .create_framebuffer = scegfx_context_opengl_create_framebuffer,
  .destroy_framebuffer = scegfx_context_opengl_destroy_framebuffer,
  .create_command_buffer = scegfx_context_opengl_create_command_buffer,
  .destroy_command_buffer = scegfx_context_opengl_destroy_command_buffer,
  .create_shader_module = scegfx_context_opengl_create_shader_module,
  .destroy_shader_module = scegfx_context_opengl_destroy_shader_module,
  .create_pipeline_layout = scegfx_context_opengl_create_pipeline_layout,
  .destroy_pipeline_layout = scegfx_context_opengl_destroy_pipeline_layout,
  .make_current = scegfx_context_opengl_make_current,
  .submit_to_queue = scegfx_context_opengl_submit_to_queue,
  .present = scegfx_context_opengl_present,
  .wait_idle = scegfx_context_opengl_wait_idle,
};

#endif // SCEGFX_CONTEXT_OPENGL_H

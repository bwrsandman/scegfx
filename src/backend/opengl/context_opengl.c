//
// Created by sandy on 21/03/19.
//

#include "context_opengl.h"

#include <assert.h>
#include <threads.h>

#include <SDL_opengl.h>
#include <SDL_video.h>

#include "buffer_opengl.h"
#include "command_buffer_opengl.h"
#include "device_memory_opengl.h"
#include "fence_opengl.h"
#include "framebuffer_opengl.h"
#include "image_opengl.h"
#include "image_view_opengl.h"
#include "render_pass_opengl.h"
#include "sampler_opengl.h"
#include "semaphore_opengl.h"
#include "swapchain_opengl.h"

#define GL_MAX_CLIENT_WAIT_TIMEOUT_WEBGL 0x9247

#if defined(EMSCRIPTEN)
thread_local EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_handle = 0;
#else
thread_local SDL_GLContext sdl_handle = NULL;
#endif

#if !defined(NDEBUG) && !defined(EMSCRIPTEN)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void
scegfx_backend_opengl_debug_callback(GLenum source,
                                     GLenum type,
                                     GLuint msg_code,
                                     GLenum severity,
                                     GLsizei length,
                                     const char* msg,
                                     const void* user_data)
{
#pragma clang diagnostic pop
  if (user_data == NULL || !msg || msg[0] == 0)
    return;
  const scegfx_context_opengl_t* this = user_data;
  scegfx_debug_severity_t level = scegfx_debug_severity_info;
  if (type == GL_DEBUG_TYPE_ERROR) {
    level = scegfx_debug_severity_error;
  } else if (severity == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) {
    level = scegfx_debug_severity_warning;
  } else if (severity == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
    level = scegfx_debug_severity_warning;
  } else if (severity == GL_DEBUG_TYPE_PORTABILITY) {
    level = scegfx_debug_severity_warning;
  } else if (severity == GL_DEBUG_TYPE_PERFORMANCE) {
    level = scegfx_debug_severity_performance_warning;
  } else if (severity == GL_DEBUG_TYPE_OTHER) {
    level = scegfx_debug_severity_info;
  } else if (severity == GL_DEBUG_TYPE_MARKER) {
    level = scegfx_debug_severity_debug;
  } else if (severity == GL_DEBUG_TYPE_PUSH_GROUP) {
    level = scegfx_debug_severity_debug;
  } else if (severity == GL_DEBUG_TYPE_POP_GROUP) {
    level = scegfx_debug_severity_debug;
  } else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
    level = scegfx_debug_severity_debug;
  }
  /*const char* severity_string = "Unknown";
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
    severity_string = "Notification";
  } else if (severity == GL_DEBUG_SEVERITY_LOW) {
    severity_string = "Low";
  } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
    severity_string = "Medium";
  } else if (severity == GL_DEBUG_SEVERITY_HIGH) {
    severity_string = "High";
  }*/
  const char* source_string = "Unknown";
  if (source == GL_DEBUG_SOURCE_API) {
    source_string = "API";
  } else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM) {
    source_string = "Windowing system";
  } else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) {
    source_string = "Shader Compiler";
  } else if (source == GL_DEBUG_SOURCE_THIRD_PARTY) {
    source_string = "Third Party";
  } else if (source == GL_DEBUG_SOURCE_APPLICATION) {
    source_string = "Application";
  } else if (source == GL_DEBUG_SOURCE_OTHER) {
    source_string = "Other";
  }

  this->super.debug_callback(
    level, msg_code, source_string, /*severity_string,*/ msg);
}
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
bool
scegfx_context_opengl_initialize(scegfx_context_t* super)
{
#pragma clang diagnostic pop
  assert(!super->initialized);
  scegfx_context_opengl_t* this = (scegfx_context_opengl_t*)super;

  char message[0x100];
#if defined(EMSCRIPTEN)
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.antialias = true;
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;
  attrs.alpha = false;
  webgl_handle = emscripten_webgl_create_context(NULL, &attrs);
  if (webgl_handle <= 0) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to create webgl context");
    return false;
  }
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(webgl_handle);
  if (res != EMSCRIPTEN_RESULT_SUCCESS) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to make context current");
    emscripten_webgl_destroy_context(webgl_handle);
    webgl_handle = 0;
    return false;
  }
#else
  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  sdl_handle = SDL_GL_CreateContext(this->super.window_handle);
  if (sdl_handle == NULL) {
    this->super.debug_callback(
      scegfx_debug_severity_error, __LINE__, FILE_BASENAME, SDL_GetError());
    return false;
  }
#endif

#if !defined(NDEBUG) && !defined(EMSCRIPTEN)
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(scegfx_backend_opengl_debug_callback, this);
#endif

#if defined(EMSCRIPTEN)
  const char* opengl_version = (const char*)glGetString(GL_VERSION);
  sprintf(message, "OpenGL Version: %s", opengl_version);
#else
  GLint major_version = 0;
  GLint minor_version = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &major_version);
  glGetIntegerv(GL_MINOR_VERSION, &minor_version);
  sprintf(message, "OpenGL Version: %d.%d", major_version, minor_version);
#endif
  this->super.debug_callback(
    scegfx_debug_severity_info, __LINE__, FILE_BASENAME, message);

  const char* glsl_version =
    (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
  sprintf(message, "GLSL Version: %s", glsl_version);
  this->super.debug_callback(
    scegfx_debug_severity_info, __LINE__, FILE_BASENAME, message);

  // Zero out
  this->extension_count = 0;
  memset(this->extension_names, 0, sizeof(this->extension_names));
  // Get available extensions
  int32_t extension_count = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);
  assert(extension_count > 0);
  this->extension_count = (uint32_t)extension_count;
  this->super.debug_callback(scegfx_debug_severity_info,
                             __LINE__,
                             FILE_BASENAME,
                             "Available extensions:");
  for (uint32_t i = 0; i < this->extension_count; ++i) {
    const char* extension_name = (const char*)glGetStringi(GL_EXTENSIONS, i);
    strncpy(this->extension_names[i],
            extension_name,
            SCEGFX_OPENGL_MAX_EXTENSION_STRING_SIZE * sizeof(char));
    this->super.debug_callback(scegfx_debug_severity_info,
                               __LINE__,
                               FILE_BASENAME,
                               this->extension_names[i]);
  }

  this->max_client_wait_timeout = UINT64_MAX;
#if defined(EMSCRIPTEN)
  glGetInteger64v(GL_MAX_CLIENT_WAIT_TIMEOUT_WEBGL,
                  (int64_t*)&this->max_client_wait_timeout);
#endif // defined(EMSCRIPTEN)
  this->max_server_wait_timeout = 0;
  glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT,
                  (int64_t*)&this->max_server_wait_timeout);

  SDL_GL_SetSwapInterval(0);

  super->initialized = true;
  return true;
}

void
scegfx_context_opengl_terminate(scegfx_context_t* super)
{
  assert(super->initialized);
#if defined(EMSCRIPTEN)
  emscripten_webgl_destroy_context(webgl_handle);
#else
  SDL_GL_DeleteContext(sdl_handle);
#endif
  super->initialized = false;
}

scegfx_fence_t*
scegfx_context_opengl_create_fence(scegfx_context_t* super,
                                   scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_context_opengl_t* this = (scegfx_context_opengl_t*)super;
  scegfx_fence_t* fence = NULL;
  if (allocator == NULL)
    fence = malloc(sizeof(scegfx_fence_opengl_t));
  else
    fence = allocator->allocator_callback(
      NULL, sizeof(scegfx_fence_opengl_t), allocator->user_data);
  memset(fence, 0, sizeof(scegfx_fence_opengl_t));

  fence->api_vtable = &scegfx_fence_api_vtable_opengl;
  fence->context = super;
  uint64_t* mutable_max_wait_timeout = (uint64_t*)&fence->max_wait_timeout;
  *mutable_max_wait_timeout = this->max_client_wait_timeout;

  return fence;
}

void
scegfx_context_opengl_destroy_fence(scegfx_context_t* this,
                                    scegfx_fence_t* fence,
                                    scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(fence);
  } else {
    allocator->allocator_callback(fence, 0, allocator->user_data);
  }
}

scegfx_semaphore_t*
scegfx_context_opengl_create_semaphore(scegfx_context_t* super,
                                       scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_context_opengl_t* this = (scegfx_context_opengl_t*)super;
  scegfx_semaphore_t* semaphore = NULL;
  if (allocator == NULL)
    semaphore = malloc(sizeof(scegfx_semaphore_opengl_t));
  else
    semaphore = allocator->allocator_callback(
      NULL, sizeof(scegfx_semaphore_opengl_t), allocator->user_data);
  memset(semaphore, 0, sizeof(scegfx_semaphore_opengl_t));

  semaphore->api_vtable = &scegfx_semaphore_api_vtable_opengl;
  semaphore->context = super;
  uint64_t* mutable_max_wait_timeout = (uint64_t*)&semaphore->max_wait_timeout;
  *mutable_max_wait_timeout = this->max_server_wait_timeout;

  return semaphore;
}

void
scegfx_context_opengl_destroy_semaphore(scegfx_context_t* this,
                                        scegfx_semaphore_t* semaphore,
                                        scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(semaphore);
  } else {
    allocator->allocator_callback(semaphore, 0, allocator->user_data);
  }
}

typedef union scegfx_opengl_memory_type_t
{
  uint32_t raw;
  struct
  {
    uint16_t usage;
    uint16_t bind_point;
  };
} scegfx_opengl_memory_type_t;
static_assert(sizeof(uint32_t) >= sizeof(scegfx_opengl_memory_type_t),
              "Memory type does not fit in 32 bits");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
uint32_t
scegfx_context_opengl_get_memory_type(scegfx_context_t* super,
                                      uint32_t type_bits,
                                      scegfx_memory_properties_t properties)
#pragma clang diagnostic pop
{
  assert(super->initialized);
  scegfx_buffer_usage_t usage = type_bits;
  scegfx_opengl_memory_type_t result = { .raw = 0 };
  if (usage & scegfx_buffer_usage_index_buffer) {
    result.usage = GL_STATIC_DRAW;
    result.bind_point = GL_ELEMENT_ARRAY_BUFFER;
  } else if (usage & scegfx_buffer_usage_vertex_buffer) {
    result.usage = GL_STATIC_DRAW;
    result.bind_point = GL_ARRAY_BUFFER;
  } else if (usage & scegfx_buffer_usage_uniform_buffer) {
    result.usage = GL_DYNAMIC_DRAW;
    result.bind_point = GL_UNIFORM_BUFFER;
  } else if (usage & scegfx_buffer_usage_transfer_src) {
    result.usage = GL_DYNAMIC_COPY;
    result.bind_point = GL_COPY_READ_BUFFER;
  } else if (usage & scegfx_buffer_usage_transfer_dst) {
    result.usage = GL_DYNAMIC_COPY;
    result.bind_point = GL_COPY_WRITE_BUFFER;
  }

  if (properties & scegfx_memory_properties_device_local) {
    result.usage = GL_STATIC_DRAW;
  } else if (properties & scegfx_memory_properties_host_visible) {
    result.usage = GL_DYNAMIC_DRAW;
  }

  assert(result.raw != 0);
  return result.raw;
}

scegfx_device_memory_t*
scegfx_context_opengl_allocate_memory(
  scegfx_context_t* super,
  const scegfx_device_memory_allocate_info_t* info,
  scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_opengl_memory_type_t memory_type = { .raw = info->memory_type_index };

  uint32_t memory_gl = 0;
  glGenBuffers(1, &memory_gl);

  scegfx_device_memory_opengl_t* memory = NULL;
  if (allocator == NULL)
    memory = malloc(sizeof(scegfx_device_memory_opengl_t));
  else
    memory = allocator->allocator_callback(
      NULL, sizeof(scegfx_device_memory_opengl_t), allocator->user_data);
  memset(memory, 0, sizeof(scegfx_device_memory_opengl_t));
  memory->handle = memory_gl;

  memory->bind_point = memory_type.bind_point;

  glBindBuffer(memory->bind_point, memory->handle);
  glBufferData(
    memory->bind_point, info->allocation_size, NULL, memory_type.usage);

#if defined(EMSCRIPTEN)
  memory->stage = malloc(info->allocation_size);
  memory->size = info->allocation_size;
#endif // defined(EMSCRIPTEN)

  memory->super.initialized = true;
  return (scegfx_device_memory_t*)memory;
}

void
scegfx_context_opengl_free_memory(scegfx_context_t* super,
                                  scegfx_device_memory_t* memory,
                                  scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  assert(memory);
  scegfx_device_memory_opengl_t* memory_gl =
    (scegfx_device_memory_opengl_t*)memory;

#if defined(EMSCRIPTEN)
  free(memory_gl->stage);
#endif // defined(EMSCRIPTEN)

  glDeleteBuffers(1, &memory_gl->handle);

  if (allocator == NULL) {
    free(memory);
  } else {
    allocator->allocator_callback(memory, 0, allocator->user_data);
  }
}

bool
scegfx_context_opengl_map_memory(scegfx_context_t* super,
                                 scegfx_device_memory_t* memory,
                                 scegfx_device_size_t offset,
                                 scegfx_device_size_t size,
                                 void** data)
{
  assert(super->initialized);
  assert(memory);
  scegfx_device_memory_opengl_t* memory_gl =
    (scegfx_device_memory_opengl_t*)memory;
  glBindBuffer(memory_gl->bind_point, memory_gl->handle);
#if defined(EMSCRIPTEN)
  assert(size);
  *data = (void*)((uintptr_t)memory_gl->stage + offset);
#else
  *data = glMapBufferRange(memory_gl->bind_point,
                           offset,
                           size,
                           GL_MAP_READ_BIT | GL_MAP_WRITE_BIT |
                             GL_MAP_FLUSH_EXPLICIT_BIT);
#endif // defined(EMSCRIPTEN)
  return *data != NULL;
}

void
scegfx_context_opengl_unmap_memory(scegfx_context_t* super,
                                   scegfx_device_memory_t* memory)
{
  assert(super->initialized);
  assert(memory);
  scegfx_device_memory_opengl_t* memory_gl =
    (scegfx_device_memory_opengl_t*)memory;
#if defined(EMSCRIPTEN)
  scegfx_mapped_device_memory_range_t range = {
    .memory = memory,
    .offset = 0,
    .size = memory_gl->size,
  };
  scegfx_context_opengl_flush_mapped_memory_ranges(super, 1, &range);
#else
  glUnmapBuffer(memory_gl->bind_point);
#endif // !defined(EMSCRIPTEN)
}

bool
scegfx_context_opengl_flush_mapped_memory_ranges(
  scegfx_context_t* super,
  uint32_t memory_range_count,
  const scegfx_mapped_device_memory_range_t* memory_ranges)
{
  assert(super->initialized);
  for (uint32_t i = 0; i < memory_range_count; ++i) {
    scegfx_device_memory_opengl_t* memory_gl =
      (scegfx_device_memory_opengl_t*)memory_ranges[i].memory;
#if defined(EMSCRIPTEN)
    void* stage_ptr =
      (void*)((uintptr_t)memory_gl->stage + memory_ranges[i].offset);
    glBufferSubData(memory_gl->bind_point,
                    memory_ranges[i].offset,
                    memory_ranges[i].size,
                    stage_ptr);
#else
    glFlushMappedBufferRange(
      memory_gl->bind_point, memory_ranges[i].offset, memory_ranges[i].size);
#endif // defined(EMSCRIPTEN)
  }
  return true;
}

scegfx_buffer_t*
scegfx_context_opengl_create_buffer(scegfx_context_t* super,
                                    scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_buffer_t* buffer = NULL;
  if (allocator == NULL)
    buffer = malloc(sizeof(scegfx_buffer_opengl_t));
  else
    buffer = allocator->allocator_callback(
      NULL, sizeof(scegfx_buffer_opengl_t), allocator->user_data);
  memset(buffer, 0, sizeof(scegfx_buffer_opengl_t));

  buffer->api_vtable = &scegfx_buffer_api_vtable_opengl;
  buffer->context = super;

  return buffer;
}

void
scegfx_context_opengl_destroy_buffer(scegfx_context_t* super,
                                     scegfx_buffer_t* buffer,
                                     scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  if (allocator == NULL) {
    free(buffer);
  } else {
    allocator->allocator_callback(buffer, 0, allocator->user_data);
  }
}

void
scegfx_context_opengl_get_buffer_memory_requirements(
  scegfx_context_t* super,
  const scegfx_buffer_t* buffer,
  scegfx_device_memory_requirements_t* memory_requirements)
{
  assert(super->initialized);
  assert(buffer);
  assert(buffer->initialized);
  scegfx_buffer_opengl_t* buffer_gl = (scegfx_buffer_opengl_t*)buffer;
  memory_requirements->size = buffer_gl->size;
  memory_requirements->memory_type_bits = buffer_gl->usage;
}

bool
scegfx_context_opengl_bind_buffer_memory(scegfx_context_t* super,
                                         scegfx_buffer_t* buffer,
                                         scegfx_device_memory_t* memory,
                                         scegfx_device_size_t memory_offset)
{
  assert(super->initialized);
  assert(memory_offset == 0);
  scegfx_buffer_opengl_t* buffer_gl = (scegfx_buffer_opengl_t*)buffer;
  scegfx_device_memory_opengl_t* memory_gl =
    (scegfx_device_memory_opengl_t*)memory;

  buffer_gl->handle = memory_gl->handle;

  return buffer_gl->handle != 0;
}

bool
scegfx_context_opengl_bind_image_memory(scegfx_context_t* super,
                                        scegfx_image_t* image,
                                        scegfx_device_memory_t* memory,
                                        scegfx_device_size_t memory_offset)
{
  assert(super->initialized);
  assert(memory);
  assert(memory_offset == 0);
  scegfx_image_opengl_t* image_gl = (scegfx_image_opengl_t*)image;

  glBindTexture(image_gl->target, image_gl->handle);
  glTexImage2D(image_gl->target,
               0,
               image_gl->format,
               image_gl->super.extent.width,
               image_gl->super.extent.height,
               0,
               image_gl->format,
               image_gl->type,
               NULL);
  glBindTexture(image_gl->target, 0);

  return true;
}

scegfx_swapchain_t*
scegfx_context_opengl_create_swapchain(scegfx_context_t* super,
                                       scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_swapchain_t* swapchain = NULL;
  if (allocator == NULL)
    swapchain = malloc(sizeof(scegfx_swapchain_opengl_t));
  else
    swapchain = allocator->allocator_callback(
      NULL, sizeof(scegfx_swapchain_opengl_t), allocator->user_data);
  memset(swapchain, 0, sizeof(scegfx_swapchain_opengl_t));

  swapchain->api_vtable = &scegfx_swapchain_api_vtable_opengl;
  swapchain->context = super;

  return swapchain;
}

void
scegfx_context_opengl_destroy_swapchain(scegfx_context_t* this,
                                        scegfx_swapchain_t* swapchain,
                                        scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(swapchain);
  } else {
    allocator->allocator_callback(swapchain, 0, allocator->user_data);
  }
}

scegfx_image_t*
scegfx_context_opengl_create_image(scegfx_context_t* super,
                                   scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_image_t* image = NULL;
  if (allocator == NULL)
    image = malloc(sizeof(scegfx_image_opengl_t));
  else
    image = allocator->allocator_callback(
      NULL, sizeof(scegfx_image_opengl_t), allocator->user_data);
  memset(image, 0, sizeof(scegfx_image_opengl_t));

  image->api_vtable = &scegfx_image_api_vtable_opengl;
  image->context = super;

  return image;
}

void
scegfx_context_opengl_destroy_image(scegfx_context_t* this,
                                    scegfx_image_t* image,
                                    scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(image);
  } else {
    allocator->allocator_callback(image, 0, allocator->user_data);
  }
}

void
scegfx_context_opengl_get_image_memory_requirements(
  const scegfx_context_t* super,
  const scegfx_image_t* image,
  scegfx_device_memory_requirements_t* memory_requirements)
{
  assert(super->initialized);
  assert(image);
  assert(image->initialized);
  const scegfx_image_opengl_t* image_gl = (const scegfx_image_opengl_t*)image;
  const uint32_t dim = image_gl->super.extent.width *
                       image_gl->super.extent.height *
                       image_gl->super.extent.depth;

  scegfx_device_size_t format_size = 0;
  switch (image_gl->super.format) {
    case scegfx_format_s8_uint:
      format_size = 0x08;
      break;
    case scegfx_format_d16_unorm_s8_uint:
      format_size = 0x18;
      break;
    case scegfx_format_r8g8b8a8_unorm:
    case scegfx_format_d32_sfloat:
    case scegfx_format_d24_unorm_s8_uint:
      format_size = 0x20;
      break;
    case scegfx_format_d32_unorm_s8_uint:
      format_size = 0x28;
      break;
    case scegfx_format_r32g32_sfloat:
      format_size = 0x40;
      break;
    case scegfx_format_r32g32b32_sfloat:
      format_size = 0x60;
      break;
    case scegfx_format_r32g32b32a32_sfloat:
      format_size = 0x80;
      break;
    default:
      assert(false);
      break;
  }

  memory_requirements->size = dim * format_size;
  memory_requirements->memory_type_bits = image_gl->super.format;
}

scegfx_render_pass_t*
scegfx_context_opengl_create_render_pass(scegfx_context_t* super,
                                         scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_render_pass_t* render_pass = NULL;
  if (allocator == NULL)
    render_pass = malloc(sizeof(scegfx_render_pass_opengl_t));
  else
    render_pass = allocator->allocator_callback(
      NULL, sizeof(scegfx_render_pass_opengl_t), allocator->user_data);
  memset(render_pass, 0, sizeof(scegfx_render_pass_opengl_t));

  render_pass->api_vtable = &scegfx_render_pass_api_vtable_opengl;
  render_pass->context = super;

  return render_pass;
}

void
scegfx_context_opengl_destroy_render_pass(scegfx_context_t* this,
                                          scegfx_render_pass_t* render_pass,
                                          scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(render_pass);
  } else {
    allocator->allocator_callback(render_pass, 0, allocator->user_data);
  }
}

scegfx_image_view_t*
scegfx_context_opengl_create_image_view(scegfx_context_t* super,
                                        scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_image_view_t* view = NULL;
  if (allocator == NULL)
    view = malloc(sizeof(scegfx_image_view_opengl_t));
  else
    view = allocator->allocator_callback(
      NULL, sizeof(scegfx_image_view_opengl_t), allocator->user_data);
  memset(view, 0, sizeof(scegfx_image_view_opengl_t));

  view->api_vtable = &scegfx_image_view_api_vtable_opengl;
  view->context = super;

  return view;
}

void
scegfx_context_opengl_destroy_image_view(scegfx_context_t* this,
                                         scegfx_image_view_t* view,
                                         scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(view);
  } else {
    allocator->allocator_callback(view, 0, allocator->user_data);
  }
}

scegfx_sampler_t*
scegfx_context_opengl_create_sampler(scegfx_context_t* super,
                                     scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_sampler_t* sampler = NULL;
  if (allocator == NULL)
    sampler = malloc(sizeof(scegfx_sampler_opengl_t));
  else
    sampler = allocator->allocator_callback(
      NULL, sizeof(scegfx_sampler_opengl_t), allocator->user_data);
  memset(sampler, 0, sizeof(scegfx_sampler_opengl_t));

  sampler->api_vtable = &scegfx_sampler_api_vtable_opengl;
  sampler->context = super;

  return sampler;
}

void
scegfx_context_opengl_destroy_sampler(scegfx_context_t* this,
                                      scegfx_sampler_t* sampler,
                                      scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(sampler);
  } else {
    allocator->allocator_callback(sampler, 0, allocator->user_data);
  }
}

scegfx_framebuffer_t*
scegfx_context_opengl_create_framebuffer(scegfx_context_t* super,
                                         scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  scegfx_framebuffer_t* framebuffer = NULL;
  if (allocator == NULL)
    framebuffer = malloc(sizeof(scegfx_framebuffer_opengl_t));
  else
    framebuffer = allocator->allocator_callback(
      NULL, sizeof(scegfx_framebuffer_opengl_t), allocator->user_data);
  memset(framebuffer, 0, sizeof(scegfx_framebuffer_opengl_t));

  framebuffer->api_vtable = &scegfx_framebuffer_api_vtable_opengl;
  framebuffer->context = super;

  return framebuffer;
}

void
scegfx_context_opengl_destroy_framebuffer(scegfx_context_t* this,
                                          scegfx_framebuffer_t* framebuffer,
                                          scegfx_allocator_t* allocator)
{
  assert(this->initialized);
  if (allocator == NULL) {
    free(framebuffer);
  } else {
    allocator->allocator_callback(framebuffer, 0, allocator->user_data);
  }
}

scegfx_command_buffer_t*
scegfx_context_opengl_create_command_buffer(scegfx_context_t* super,
                                            scegfx_allocator_t* allocator)
{
  if (!super->initialized)
    return NULL;

  scegfx_command_buffer_t* command_buffer;
  if (allocator == NULL)
    command_buffer = malloc(sizeof(scegfx_command_buffer_opengl_t));
  else
    command_buffer = allocator->allocator_callback(
      NULL, sizeof(scegfx_command_buffer_opengl_t), allocator->user_data);
  memset(command_buffer, 0, sizeof(scegfx_command_buffer_opengl_t));

  command_buffer->api_vtable = &scegfx_command_buffer_api_vtable_opengl;
  command_buffer->context = super;

  return command_buffer;
}

void
scegfx_context_opengl_destroy_command_buffer(scegfx_context_t* super,
                                             scegfx_command_buffer_t* queue,
                                             scegfx_allocator_t* allocator)
{
  assert(super->initialized);
  assert(!queue->initialized);
  if (allocator == NULL) {
    free(queue);
  } else {
    allocator->allocator_callback(queue, 0, allocator->user_data);
  }
}

bool
scegfx_context_opengl_make_current(scegfx_context_t* super)
{
  assert(super->initialized);
  scegfx_context_opengl_t* this = (scegfx_context_opengl_t*)super;
#if defined(EMSCRIPTEN)
  if (webgl_handle == 0) {
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.antialias = true;
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.alpha = false;
    webgl_handle = emscripten_webgl_create_context(NULL, &attrs);
    if (webgl_handle <= 0) {
      this->super.debug_callback(scegfx_debug_severity_error,
                                 __LINE__,
                                 FILE_BASENAME,
                                 "Unable to create webgl context");
      return false;
    }
  }
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(webgl_handle);
  if (res != EMSCRIPTEN_RESULT_SUCCESS) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to make context current");
    emscripten_webgl_destroy_context(webgl_handle);
    webgl_handle = 0;
    return false;
  }
#else
  if (sdl_handle == NULL) {
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    sdl_handle = SDL_GL_CreateContext(this->super.window_handle);
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(scegfx_backend_opengl_debug_callback, this);

  int res = SDL_GL_MakeCurrent(this->super.window_handle, sdl_handle);
  if (res != 0) {
    this->super.debug_callback(
      scegfx_debug_severity_error, __LINE__, FILE_BASENAME, SDL_GetError());
    return false;
  }
#endif
  return true;
}

bool
scegfx_context_opengl_submit_to_queue(scegfx_context_t* super,
                                      scegfx_submit_info_t* info,
                                      scegfx_fence_t* fence)
{
  assert(super->initialized);
  scegfx_command_buffer_opengl_t* command_buffer =
    (scegfx_command_buffer_opengl_t*)info->command_buffer;
  scegfx_semaphore_opengl_t* signal_semaphore =
    (scegfx_semaphore_opengl_t*)info->signal_semaphore;
  scegfx_semaphore_opengl_t* wait_semaphore =
    (scegfx_semaphore_opengl_t*)info->wait_semaphore;
  scegfx_fence_opengl_t* fence_gl = (scegfx_fence_opengl_t*)fence;

  // Insert a GPU wait in the command queue
  if (wait_semaphore) {
    assert(scegfx_semaphore_opengl_wait(wait_semaphore));
  }

  // Execute command buffer commands
  assert(scegfx_command_buffer_opengl_execute(command_buffer));

  // Unblock GPU wait in command queue
  if (signal_semaphore) {
    assert(scegfx_semaphore_opengl_signal(signal_semaphore));
  }

  // Insert CPU fence sync in the command queue
  assert(scegfx_fence_opengl_signal(fence_gl));

  // End of opengl commands
  glFinish();
  glFlush();

  return true;
}

bool
scegfx_context_opengl_present(scegfx_context_t* super,
                              scegfx_present_info_t* info)
{
  assert(super->initialized);
  scegfx_semaphore_opengl_t* wait_semaphore =
    (scegfx_semaphore_opengl_t*)info->wait_semaphore;
  assert(scegfx_semaphore_opengl_wait(wait_semaphore));
  SDL_GL_SwapWindow(super->window_handle);
  return true;
}

bool
scegfx_context_opengl_wait_idle(scegfx_context_t* super)
{
  assert(super->initialized);
  return true;
}

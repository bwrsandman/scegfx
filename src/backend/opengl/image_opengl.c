//
// Created by sandy on 12/06/19.
//

#include "image_opengl.h"

#include <assert.h>

#include <SDL_opengl.h>

#include "context_opengl.h"

bool
scegfx_image_opengl_initialize(scegfx_image_t* super,
                               const scegfx_image_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_image_opengl_t* this = (scegfx_image_opengl_t*)super;

  glGenTextures(1, &this->handle);

  this->super.type = info->image_type;
  switch (info->image_type) {
    case scegfx_image_type_1d:
      this->target = GL_TEXTURE_1D;
      break;
    case scegfx_image_type_2d:
      this->target = GL_TEXTURE_2D;
      break;
    case scegfx_image_type_3d:
      this->target = GL_TEXTURE_3D;
      break;
    default:
      assert(false);
      break;
  }
  switch (info->format) {
    case scegfx_format_r8g8b8a8_unorm:
      this->format = GL_RGBA;
      this->type = GL_UNSIGNED_BYTE;
      break;
    case scegfx_format_r32g32_sfloat:
      this->format = GL_RG;
      this->type = GL_FLOAT;
      break;
    case scegfx_format_r32g32b32_sfloat:
      this->format = GL_RGB;
      this->type = GL_FLOAT;
      break;
    case scegfx_format_r32g32b32a32_sfloat:
      this->format = GL_RGBA;
      this->type = GL_FLOAT;
      break;
    case scegfx_format_d32_sfloat:
      this->format = GL_DEPTH_COMPONENT;
      this->type = GL_UNSIGNED_BYTE;
      break;
    case scegfx_format_s8_uint:
      this->format = GL_R;
      this->type = GL_UNSIGNED_BYTE;
      break;
    case scegfx_format_d24_unorm_s8_uint:
      this->format = GL_DEPTH24_STENCIL8;
      this->type = GL_UNSIGNED_INT_24_8;
      break;
    case scegfx_format_d32_unorm_s8_uint:
      this->format = GL_DEPTH32F_STENCIL8;
      this->type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
      break;
    case scegfx_format_d16_unorm_s8_uint:
    default:
      assert(false);
      break;
  }

  this->super.format = info->format;
  this->super.extent = info->extent;

  this->super.initialized = true;

  return true;
}

void
scegfx_image_opengl_terminate(scegfx_image_t* super)
{
  scegfx_image_opengl_t* this = (scegfx_image_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;
  if (this->is_default_framebuffer) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Attempt to delete default back buffer image");
    assert(false);
    return;
  }
  glDeleteTextures(1, &this->handle);
}

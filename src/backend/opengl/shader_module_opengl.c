//
// Created by sandy on 09/05/19.
//

#include "shader_module_opengl.h"

#include <assert.h>

#include <SDL_opengl.h>

#include "context_opengl.h"

uint32_t
stage_to_gl_type(scegfx_stage_type_t type)
{
  switch (type) {
    case scegfx_stage_type_vertex:
      return GL_VERTEX_SHADER;
    case scegfx_stage_type_tessellation_control:
      return GL_TESS_CONTROL_SHADER;
    case scegfx_stage_type_tessellation_evaluation:
      return GL_TESS_EVALUATION_SHADER;
    case scegfx_stage_type_geometry:
      return GL_GEOMETRY_SHADER;
    case scegfx_stage_type_fragment:
      return GL_FRAGMENT_SHADER;
    case scegfx_stage_type_compute:
      return GL_COMPUTE_SHADER;
    default:
      assert(false);
      break;
  }
}

bool
scegfx_shader_module_opengl_initialize(
  scegfx_shader_module_t* super,
  const scegfx_shader_module_create_info_t* info)
{
  assert(!super->initialized);
  assert(info);
  assert(info->entry_point);
  scegfx_shader_module_opengl_t* this = (scegfx_shader_module_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;

  spvc_result result;
  spvc_parsed_ir ir = NULL;
  result = spvc_context_parse_spirv(
    context->spvc,
    (const SpvId*)info->shader_src,
    info->size / (sizeof(SpvId) / sizeof(info->shader_src[0])),
    &ir);
  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to parse SPIR-V");
    spvc_context_release_allocations(context->spvc);
    return false;
  }

  spvc_compiler compiler_glsl = NULL;
  result = spvc_context_create_compiler(context->spvc,
                                        SPVC_BACKEND_GLSL,
                                        ir,
                                        SPVC_CAPTURE_MODE_TAKE_OWNERSHIP,
                                        &compiler_glsl);
  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create SPIR-V cross-compiler");
    spvc_context_release_allocations(context->spvc);
    return false;
  }

  spvc_compiler_options spvc_options = NULL;
  result = spvc_compiler_create_compiler_options(compiler_glsl, &spvc_options);
  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create SPIR-V compiler options");
    spvc_context_release_allocations(context->spvc);
    return false;
  }

  if (info->fix_vertex_y) {
    result = spvc_compiler_options_set_bool(
      spvc_options, SPVC_COMPILER_OPTION_FLIP_VERTEX_Y, true);
    if (result != SPVC_SUCCESS) {
      context->super.debug_callback(
        scegfx_debug_severity_error,
        __LINE__,
        FILE_BASENAME,
        "Unable to set SPIR-V Y-flip compiler option");
      spvc_context_release_allocations(context->spvc);
      return false;
    }
  }

#if defined(EMSCRIPTEN)
  result = spvc_compiler_options_set_uint(
    spvc_options, SPVC_COMPILER_OPTION_GLSL_VERSION, 300);
  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to set SPIR-V GLSL version compiler option");
    spvc_context_release_allocations(context->spvc);
    return false;
  }

  result = spvc_compiler_options_set_bool(
    spvc_options, SPVC_COMPILER_OPTION_GLSL_ES, true);
  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to set SPIR-V GLSL ES compiler option");
    spvc_context_release_allocations(context->spvc);
    return false;
  }
#else
  result = spvc_compiler_options_set_uint(
    spvc_options, SPVC_COMPILER_OPTION_GLSL_VERSION, 460);
  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to set SPIR-V GLSL version compiler option");
    spvc_context_release_allocations(context->spvc);
    return false;
  }

  result = spvc_compiler_options_set_bool(
    spvc_options, SPVC_COMPILER_OPTION_GLSL_ENABLE_420PACK_EXTENSION, true);

  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Unable to set SPIR-V 420 pack extension compiler option");
    spvc_context_release_allocations(context->spvc);
    return false;
  }
#endif // defined(EMSCRIPTEN)

  result = spvc_compiler_install_compiler_options(compiler_glsl, spvc_options);
  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to install compiler options");
    spvc_context_release_allocations(context->spvc);
    return false;
  }

  const char* shader_cross_src = NULL;
  result = spvc_compiler_compile(compiler_glsl, &shader_cross_src);
  if (result != SPVC_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to cross compile SPIR-V");
    spvc_context_release_allocations(context->spvc);
    return false;
  }

  context->super.debug_callback(
    scegfx_debug_severity_debug, __LINE__, FILE_BASENAME, shader_cross_src);

  uint32_t handle = glCreateShader(stage_to_gl_type(info->type));
  if (handle == 0) {
    spvc_context_release_allocations(context->spvc);
    return false;
  }

  glShaderSource(handle, 1, &shader_cross_src, NULL);
  glCompileShader(handle);
  spvc_context_release_allocations(context->spvc);

  int32_t status;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    int32_t log_length = 0;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length && log_length < SCEGFX_OPENGL_MAX_LOG_LENGTH) {
      char log[SCEGFX_OPENGL_MAX_LOG_LENGTH];
      glGetShaderInfoLog(handle, log_length, 0, log);
      log[log_length] = 0;
      context->super.debug_callback(
        scegfx_debug_severity_error, __LINE__, FILE_BASENAME, log);
    } else {
      context->super.debug_callback(scegfx_debug_severity_error,
                                    __LINE__,
                                    FILE_BASENAME,
                                    "Unable to compile shader");
    }
    glDeleteShader(handle);
    return false;
  }

  this->handle = handle;

  this->super.initialized = true;

  return true;
}

void
scegfx_shader_module_opengl_terminate(scegfx_shader_module_t* super)
{
  assert(super->initialized);
  scegfx_shader_module_opengl_t* this = (scegfx_shader_module_opengl_t*)super;
  if (this->handle) {
    glDeleteShader(this->handle);
  }
  super->initialized = false;
}

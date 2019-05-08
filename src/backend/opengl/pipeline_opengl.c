//
// Created by sandy on 10/05/19.
//

#include "pipeline_opengl.h"

#include <assert.h>

#include <SDL_opengl.h>

#include <scegfx/pipeline.h>

#include "context_opengl.h"
#include "shader_module_opengl.h"

bool
scegfx_pipeline_opengl_initialize(scegfx_pipeline_t* super,
                                  const scegfx_pipeline_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_pipeline_opengl_t* this = (scegfx_pipeline_opengl_t*)super;
  scegfx_context_opengl_t* context = (scegfx_context_opengl_t*)super->context;

  uint32_t program = glCreateProgram();

  if (info->type == scegfx_pipeline_type_graphics) {
    scegfx_shader_module_opengl_t* vertex_shader =
      (scegfx_shader_module_opengl_t*)info->graphics.stages.vertex;
    scegfx_shader_module_opengl_t* tess_control_shader =
      (scegfx_shader_module_opengl_t*)
        info->graphics.stages.tessellation_control;
    scegfx_shader_module_opengl_t* tess_evaluation_shader =
      (scegfx_shader_module_opengl_t*)
        info->graphics.stages.tessellation_evaluation;
    scegfx_shader_module_opengl_t* geometry_shader =
      (scegfx_shader_module_opengl_t*)info->graphics.stages.geometry;
    scegfx_shader_module_opengl_t* fragment_shader =
      (scegfx_shader_module_opengl_t*)info->graphics.stages.fragment;
    if (vertex_shader) {
      glAttachShader(program, vertex_shader->handle);
    }
    if (tess_control_shader) {
      glAttachShader(program, tess_control_shader->handle);
    }
    if (tess_evaluation_shader) {
      glAttachShader(program, tess_evaluation_shader->handle);
    }
    if (geometry_shader) {
      glAttachShader(program, geometry_shader->handle);
    }
    if (fragment_shader) {
      glAttachShader(program, fragment_shader->handle);
    }
    this->graphics.topology = GL_TRIANGLES;
    switch (info->graphics.raster_state.cull_mode) {
      case scegfx_raster_state_cull_mode_none:
        this->graphics.cull_face = 0;
        break;
      case scegfx_raster_state_cull_mode_front:
        this->graphics.cull_face = GL_FRONT;
        break;
      case scegfx_raster_state_cull_mode_back:
        this->graphics.cull_face = GL_BACK;
        break;
      case scegfx_raster_state_cull_mode_front_and_back:
        this->graphics.cull_face = GL_FRONT_AND_BACK;
        break;
      default:
        assert(false);
        break;
    }
    switch (info->graphics.raster_state.front_face) {
      case scegfx_raster_state_front_face_clockwise:
        this->graphics.front_face = GL_CW;
        break;
      case scegfx_raster_state_front_face_counter_clockwise:
        this->graphics.front_face = GL_CCW;
        break;
      default:
        assert(false);
        break;
    }
    this->graphics.line_width = info->graphics.raster_state.line_width;
  } else if (info->type == scegfx_pipeline_type_compute) {
    scegfx_shader_module_opengl_t* shader =
      (scegfx_shader_module_opengl_t*)info->compute.stage;
    glAttachShader(program, shader->handle);
  } else {
    return false;
  }

  glLinkProgram(program);

  int32_t status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    int32_t log_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length && log_length < SCEGFX_OPENGL_MAX_LOG_LENGTH) {
      char log[SCEGFX_OPENGL_MAX_LOG_LENGTH];
      glGetProgramInfoLog(program, log_length, 0, log);
      log[log_length] = 0;
      context->super.debug_callback(
        scegfx_debug_severity_error, __LINE__, FILE_BASENAME, log);
    } else {
      context->super.debug_callback(scegfx_debug_severity_error,
                                    __LINE__,
                                    FILE_BASENAME,
                                    "Unable to link program");
    }
    glDeleteProgram(program);
    return false;
  }

  this->type = info->type;
  this->program = program;

  this->super.initialized = true;

  return true;
}

void
scegfx_pipeline_opengl_terminate(scegfx_pipeline_t* super)
{
  assert(super->initialized);
  scegfx_pipeline_opengl_t* this = (scegfx_pipeline_opengl_t*)super;
  if (this->program) {
    glDeleteProgram(this->program);
  }
  super->initialized = false;
}

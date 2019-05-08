//
// Created by sandy on 09/05/19.
//

#ifndef SCEGFX_SHADER_MODULE_OPENGL_H
#define SCEGFX_SHADER_MODULE_OPENGL_H

#include <scegfx/shader_module.h>

#include <stdint.h>

enum
{
  SCEGFX_OPENGL_MAX_LOG_LENGTH = 0x400,
};

typedef struct scegfx_shader_module_opengl_t
{
  scegfx_shader_module_t super;
  uint32_t handle;
} scegfx_shader_module_opengl_t;

bool
scegfx_shader_module_opengl_initialize(
  scegfx_shader_module_t* super,
  const scegfx_shader_module_create_info_t* info);
void
scegfx_shader_module_opengl_terminate(scegfx_shader_module_t* this);

const static scegfx_shader_module_api_vtable_t
  scegfx_shader_module_api_vtable_opengl = {
    .initialize = scegfx_shader_module_opengl_initialize,
    .terminate = scegfx_shader_module_opengl_terminate,
  };

#endif // SCEGFX_SHADER_MODULE_OPENGL_H

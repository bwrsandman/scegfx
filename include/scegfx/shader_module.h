//
// Created by sandy on 06/05/19.
//

#ifndef SCEGFX_SHADER_MODULE_H
#define SCEGFX_SHADER_MODULE_H

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

typedef struct scegfx_shader_module_t scegfx_shader_module_t;
typedef struct scegfx_context_t scegfx_context_t;

typedef struct scegfx_shader_module_create_info_t
{
  scegfx_stage_type_t type;
  /// Normalize Y flip between opengl (flipped) and vulkan (not flipped) in the
  /// vertex shader itself.  It is not recommended to use this option, rather
  /// use swapchain flip_y when constructing projection matrix.
  bool fix_vertex_y;
  const char* entry_point;
  uint64_t size;
  const char* shader_src;
} scegfx_shader_module_create_info_t;

typedef struct scegfx_shader_module_api_vtable_t
{
  bool (*initialize)(scegfx_shader_module_t* super,
                     const scegfx_shader_module_create_info_t* info);
  void (*terminate)(scegfx_shader_module_t* this);
} scegfx_shader_module_api_vtable_t;

typedef struct scegfx_shader_module_t
{
  bool initialized;
  const scegfx_shader_module_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_shader_module_t;

#endif // SCEGFX_SHADER_MODULE_H

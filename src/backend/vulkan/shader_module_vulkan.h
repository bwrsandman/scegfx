//
// Created by sandy on 06/05/19.
//

#ifndef SCEGFX_SHADER_MODULE_VULKAN_H
#define SCEGFX_SHADER_MODULE_VULKAN_H

#include <scegfx/shader_module.h>

#include <vulkan/vulkan.h>

enum
{
  SCEGFX_ENTRY_POINT_NAME_LIMIT = 0x100,
};

typedef struct scegfx_shader_module_vulkan_t
{
  scegfx_shader_module_t super;
  scegfx_stage_type_t type;
  char entry_point[SCEGFX_ENTRY_POINT_NAME_LIMIT];
  VkShaderModule handle;
} scegfx_shader_module_vulkan_t;

bool
scegfx_shader_module_vulkan_initialize(
  scegfx_shader_module_t* this,
  const scegfx_shader_module_create_info_t* info);
void
scegfx_shader_module_vulkan_terminate(scegfx_shader_module_t* this);

const static scegfx_shader_module_api_vtable_t
  scegfx_shader_module_api_vtable_vulkan = {
    .initialize = scegfx_shader_module_vulkan_initialize,
    .terminate = scegfx_shader_module_vulkan_terminate,
  };

#endif // SCEGFX_SHADER_MODULE_VULKAN_H

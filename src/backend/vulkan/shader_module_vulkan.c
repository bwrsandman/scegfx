//
// Created by sandy on 06/05/19.
//

#include "shader_module_vulkan.h"

#include <assert.h>
#include <string.h>

#include "context_vulkan.h"

bool
scegfx_shader_module_vulkan_initialize(
  scegfx_shader_module_t* super,
  const scegfx_shader_module_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_shader_module_vulkan_t* this = (scegfx_shader_module_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkShaderModuleCreateInfo info_vk = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = info->size,
    .pCode = (const uint32_t*)info->shader_src,
  };

  VkResult result =
    vkCreateShaderModule(context->device, &info_vk, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create shader module");
    return false;
  }

  this->type = info->type;
  memcpy(
    this->entry_point, info->entry_point, SCEGFX_ENTRY_POINT_NAME_LIMIT - 1);
  this->entry_point[SCEGFX_ENTRY_POINT_NAME_LIMIT - 1] = 0;
  this->type = info->type;

  this->super.initialized = true;
  return true;
}

void
scegfx_shader_module_vulkan_terminate(scegfx_shader_module_t* super)
{
  assert(super->initialized);
  scegfx_shader_module_vulkan_t* this = (scegfx_shader_module_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  vkDestroyShaderModule(context->device, this->handle, NULL);
  super->initialized = false;
}

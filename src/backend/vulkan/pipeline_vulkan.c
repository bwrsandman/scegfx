//
// Created by sandy on 06/05/19.
//

#include "pipeline_vulkan.h"

#include <SDL_vulkan.h>
#include <assert.h>
#include <scegfx/pipeline.h>

#include "context_vulkan.h"
#include "pipeline_layout_vulkan.h"
#include "render_pass_vulkan.h"
#include "shader_module_vulkan.h"

VkShaderStageFlagBits
stage_to_vk_type(scegfx_stage_type_t type)
{
  switch (type) {
    case scegfx_stage_type_vertex:
      return VK_SHADER_STAGE_VERTEX_BIT;
    case scegfx_stage_type_tessellation_control:
      return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case scegfx_stage_type_tessellation_evaluation:
      return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    case scegfx_stage_type_geometry:
      return VK_SHADER_STAGE_GEOMETRY_BIT;
    case scegfx_stage_type_fragment:
      return VK_SHADER_STAGE_FRAGMENT_BIT;
    case scegfx_stage_type_compute:
      return VK_SHADER_STAGE_COMPUTE_BIT;
    case scegfx_stage_type_raygen:
      return VK_SHADER_STAGE_RAYGEN_BIT_NV;
    case scegfx_stage_type_any_hit:
      return VK_SHADER_STAGE_ANY_HIT_BIT_NV;
    case scegfx_stage_type_closest_hit:
      return VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;
    case scegfx_stage_type_miss:
      return VK_SHADER_STAGE_MISS_BIT_NV;
    case scegfx_stage_type_intersection:
      return VK_SHADER_STAGE_INTERSECTION_BIT_NV;
    default:
      assert(false);
      break;
  }
}

bool
scegfx_pipeline_vulkan_initialize(scegfx_pipeline_t* super,
                                  const scegfx_pipeline_create_info_t* info)
{
  VkResult result;
  assert(!super->initialized);
  scegfx_pipeline_vulkan_t* this = (scegfx_pipeline_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  scegfx_pipeline_layout_vulkan_t* layout =
    (scegfx_pipeline_layout_vulkan_t*)info->layout;

  assert(layout);

  VkPipelineShaderStageCreateInfo stages[5];

  if (info->type == scegfx_pipeline_type_graphics) {
    scegfx_render_pass_vulkan_t* render_pass =
      (scegfx_render_pass_vulkan_t*)info->graphics.render_pass;
    uint32_t stage_count = 0;
    for (uint32_t i = 0; i < 5; ++i) {
      scegfx_shader_module_vulkan_t* stage =
        (scegfx_shader_module_vulkan_t*)info->graphics.stages._all_stages[i];
      if (stage == NULL) {
        continue;
      }
      stages[stage_count].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      stages[stage_count].flags = 0;
      stages[stage_count].pNext = NULL;
      stages[stage_count].stage = stage_to_vk_type(stage->type);
      stages[stage_count].module = stage->handle;
      stages[stage_count].pName = stage->entry_point;
      stages[stage_count].pSpecializationInfo = NULL;
      ++stage_count;
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };
    const VkVertexInputBindingDescription binding = {
      .binding = 0,
      .stride = info->graphics.vertex_input_state.binding_description.stride,
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    assert(info->graphics.vertex_input_state.attribute_description_count <=
           SCEGFX_VULKAN_MAX_VERTEX_ATTRIBUTE_COUNT);
    VkVertexInputAttributeDescription
      attributes[SCEGFX_VULKAN_MAX_VERTEX_ATTRIBUTE_COUNT];
    for (uint32_t i = 0;
         i < info->graphics.vertex_input_state.attribute_description_count;
         ++i) {
      attributes[i].location =
        info->graphics.vertex_input_state.attribute_descriptions[i].location;
      attributes[i].binding = 0;
      attributes[i].format =
        (VkFormat)info->graphics.vertex_input_state.attribute_descriptions[i]
          .format;
      attributes[i].offset =
        info->graphics.vertex_input_state.attribute_descriptions[i].offset;
    }
    if (info->graphics.vertex_input_state.attribute_description_count) {
      vertex_input_state.vertexBindingDescriptionCount = 1;
      vertex_input_state.pVertexBindingDescriptions = &binding;
      vertex_input_state.vertexAttributeDescriptionCount =
        info->graphics.vertex_input_state.attribute_description_count;
      vertex_input_state.pVertexAttributeDescriptions = attributes;
    }
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };
    int width, height;
    SDL_Vulkan_GetDrawableSize(context->super.window_handle, &width, &height);
    VkViewport viewport = {
      .x = 0,
      .y = 0,
      .width = width,
      .height = height,
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
    };
    VkRect2D scissor = {
        .offset = {
            .x = 0,
            .y = 0,
        },
        .extent = {
            .width = width,
            .height = height,
        },
    };
    VkPipelineViewportStateCreateInfo viewport_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissor,
    };
    VkPipelineRasterizationStateCreateInfo raster_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .cullMode = info->graphics.raster_state.cull_mode,
      .frontFace = (VkFrontFace)info->graphics.raster_state.front_face,
      .lineWidth = info->graphics.raster_state.line_width,
    };
    VkPipelineMultisampleStateCreateInfo multisample = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = 1,
    };
    VkPipelineColorBlendAttachmentState blend_attachment = {
      .blendEnable = false,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
    VkPipelineColorBlendStateCreateInfo color_blend = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &blend_attachment,
    };
    VkGraphicsPipelineCreateInfo graphics_info = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = stage_count,
      .pStages = stages,
      .pVertexInputState = &vertex_input_state,
      .pInputAssemblyState = &input_assembly,
      //        .pTessellationState = ,
      .pViewportState = &viewport_state,
      .pRasterizationState = &raster_state,
      .pMultisampleState = &multisample,
      //        .pDepthStencilState = ,
      .pColorBlendState = &color_blend,
      //        .pDynamicState = &dynamic_state,
      .layout = layout->handle,
      .renderPass = render_pass->handle,
      .subpass = 0,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
    };
    result = vkCreateGraphicsPipelines(context->device,
                                       context->pipeline_cache,
                                       1,
                                       &graphics_info,
                                       NULL,
                                       &this->handle);
  } else if (info->type == scegfx_pipeline_type_compute) {
    scegfx_shader_module_vulkan_t* stage =
      (scegfx_shader_module_vulkan_t*)info->compute.stage;
    assert(stage);
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].flags = 0;
    stages[0].pNext = NULL;
    stages[0].stage = stage_to_vk_type(stage->type);
    stages[0].module = stage->handle;
    stages[0].pName = stage->entry_point;
    stages[0].pSpecializationInfo = NULL;
    VkComputePipelineCreateInfo compute_info = {
      .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
      .stage = stages[0],
      .layout = layout->handle,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
    };
    result = vkCreateComputePipelines(context->device,
                                      context->pipeline_cache,
                                      1,
                                      &compute_info,
                                      NULL,
                                      &this->handle);
  } else if (info->type == scegfx_pipeline_type_ray_tracing &&
             context->functions.CreateRayTracingPipelines) {
    uint32_t stage_count = 0;
    for (uint32_t i = 0; i < 5; ++i) {
      scegfx_shader_module_vulkan_t* stage =
        (scegfx_shader_module_vulkan_t*)info->ray_tracing.stages._all_stages[i];
      if (stage == NULL) {
        continue;
      }
      stages[stage_count].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      stages[stage_count].flags = 0;
      stages[stage_count].pNext = NULL;
      stages[stage_count].stage = stage_to_vk_type(stage->type);
      stages[stage_count].module = stage->handle;
      stages[stage_count].pName = stage->entry_point;
      stages[stage_count].pSpecializationInfo = NULL;
      ++stage_count;
    }
    VkRayTracingPipelineCreateInfoNV ray_tracing_info = {
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV,
      .stageCount = stage_count,
      .pStages = stages,
      //        .groupCount = info->ray_tracing.group_count,
      //        .pGroups = info->ray_tracing.groups,
      .maxRecursionDepth = info->ray_tracing.max_recursion_depth,
      .layout = layout->handle,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
    };
    result =
      context->functions.CreateRayTracingPipelines(context->device,
                                                   context->pipeline_cache,
                                                   1,
                                                   &ray_tracing_info,
                                                   NULL,
                                                   &this->handle);
  } else {
    assert(false);
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Invalid pipeline type");
    return false;
  }

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create pipeline");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_pipeline_vulkan_terminate(scegfx_pipeline_t* super)
{
  assert(super->initialized);
  scegfx_pipeline_vulkan_t* this = (scegfx_pipeline_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  vkDestroyPipeline(context->device, this->handle, NULL);
  super->initialized = false;
}

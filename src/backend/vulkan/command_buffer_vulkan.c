//
// Created by sandy on 20/04/19.
//

#include "command_buffer_vulkan.h"

#include <assert.h>
#include <string.h>

#include "buffer_vulkan.h"
#include "context_vulkan.h"
#include "framebuffer_vulkan.h"
#include "image_vulkan.h"
#include "pipeline_vulkan.h"
#include "render_pass_vulkan.h"

bool
scegfx_command_buffer_vulkan_initialize(scegfx_command_buffer_t* super)
{
  assert(!super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkCommandBufferAllocateInfo info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = context->command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  VkResult result =
    vkAllocateCommandBuffers(context->device, &info, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to create command buffer");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_command_buffer_vulkan_terminate(scegfx_command_buffer_t* super)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  vkFreeCommandBuffers(
    context->device, context->command_pool, 1, &this->handle);
  super->initialized = false;
}

bool
scegfx_command_buffer_vulkan_begin(scegfx_command_buffer_t* super,
                                   bool long_lived)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  VkCommandBufferBeginInfo info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = long_lived ? 0 : VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };
  VkResult result = vkBeginCommandBuffer(this->handle, &info);
  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to begin command buffer");
    return false;
  }
  return true;
}

bool
scegfx_command_buffer_vulkan_end(scegfx_command_buffer_t* super)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkResult result = vkEndCommandBuffer(this->handle);
  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to end command buffer");
    return false;
  }
  return true;
}

void
scegfx_command_buffer_vulkan_begin_render_pass(
  scegfx_command_buffer_t* super,
  const scegfx_render_pass_begin_info_t* info)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_render_pass_vulkan_t* render_pass =
    (scegfx_render_pass_vulkan_t*)info->render_pass;
  scegfx_framebuffer_vulkan_t* framebuffer =
    (scegfx_framebuffer_vulkan_t*)info->framebuffer;

  VkClearValue clear_values[2];
  for (uint32_t i = 0; i < 4; ++i) {
    clear_values[0].color.float32[i] = info->clear_values.color[i];
  }
  clear_values[1].depthStencil.depth = info->clear_values.depth;

  VkRenderPassBeginInfo vk_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .renderPass = render_pass->handle,
    .framebuffer = framebuffer->handle,
    .renderArea = *(VkRect2D*)&info->render_area,
    .clearValueCount = sizeof(clear_values) / sizeof(clear_values[0]),
    .pClearValues = clear_values,
  };

  vkCmdBeginRenderPass(this->handle, &vk_info, VK_SUBPASS_CONTENTS_INLINE);
}

void
scegfx_command_buffer_vulkan_end_render_pass(scegfx_command_buffer_t* super)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;

  vkCmdEndRenderPass(this->handle);
}

void
scegfx_command_buffer_vulkan_bind_pipeline(scegfx_command_buffer_t* super,
                                           scegfx_pipeline_type_t type,
                                           const scegfx_pipeline_t* pipeline)
{
  assert(super->initialized);

  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_pipeline_vulkan_t* pipeline_vk = (scegfx_pipeline_vulkan_t*)pipeline;

  VkPipelineBindPoint bind_point;

  switch (type) {
    case scegfx_pipeline_type_graphics:
      bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
      break;
    case scegfx_pipeline_type_compute:
      bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
      break;
    case scegfx_pipeline_type_ray_tracing:
      bind_point = VK_PIPELINE_BIND_POINT_RAY_TRACING_NV;
      break;
    default:
      assert(0);
      return;
  }

  vkCmdBindPipeline(this->handle, bind_point, pipeline_vk->handle);
}

void
scegfx_command_buffer_vulkan_bind_vertex_buffer(scegfx_command_buffer_t* super,
                                                const scegfx_buffer_t* buffer,
                                                scegfx_device_size_t offset)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;

  VkBuffer vk_buffer;
  assert(buffer->initialized);
  vk_buffer = ((scegfx_buffer_vulkan_t*)buffer)->handle;

  vkCmdBindVertexBuffers(this->handle, 0, 1, &vk_buffer, &offset);
}

void
scegfx_command_buffer_vulkan_bind_index_buffer(
  scegfx_command_buffer_t* super,
  const scegfx_buffer_t* buffer,
  const scegfx_device_size_t offset,
  scegfx_index_type_t index_type)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  assert(buffer->initialized);
  scegfx_buffer_vulkan_t* vk_buffer = (scegfx_buffer_vulkan_t*)buffer;

  VkIndexType vk_index_type = VK_INDEX_TYPE_UINT32;
  switch (index_type) {
    case scegfx_index_type_u16:
      vk_index_type = VK_INDEX_TYPE_UINT16;
      break;
    case scegfx_index_type_u32:
      vk_index_type = VK_INDEX_TYPE_UINT32;
      break;
    default:
      assert(0);
      break;
  }

  vkCmdBindIndexBuffer(this->handle, vk_buffer->handle, offset, vk_index_type);
}

void
scegfx_command_buffer_vulkan_draw(scegfx_command_buffer_t* super,
                                  uint32_t vertex_count,
                                  uint32_t instance_count,
                                  uint32_t first_vertex,
                                  uint32_t first_instance)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  vkCmdDraw(
    this->handle, vertex_count, instance_count, first_vertex, first_instance);
}

void
scegfx_command_buffer_vulkan_draw_indexed(scegfx_command_buffer_t* super,
                                          uint32_t index_count,
                                          uint32_t instance_count,
                                          uint32_t first_index,
                                          int32_t vertex_offset,
                                          uint32_t first_instance)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  vkCmdDrawIndexed(this->handle,
                   index_count,
                   instance_count,
                   first_index,
                   vertex_offset,
                   first_instance);
}

void
scegfx_command_buffer_vulkan_copy_buffer(scegfx_command_buffer_t* super,
                                         const scegfx_buffer_t* src_buffer,
                                         const scegfx_buffer_t* dst_buffer,
                                         const scegfx_buffer_copy_t* region)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_buffer_vulkan_t* vk_src_buffer = (scegfx_buffer_vulkan_t*)src_buffer;
  scegfx_buffer_vulkan_t* vk_dst_buffer = (scegfx_buffer_vulkan_t*)dst_buffer;

  VkBufferCopy vk_region = {
    .srcOffset = region->src_offset,
    .dstOffset = region->dst_offset,
    .size = region->size,
  };

  vkCmdCopyBuffer(
    this->handle, vk_src_buffer->handle, vk_dst_buffer->handle, 1, &vk_region);
}

void
scegfx_command_buffer_vulkan_copy_image(scegfx_command_buffer_t* super,
                                        const scegfx_image_t* src_image,
                                        scegfx_image_layout_t src_image_layout,
                                        const scegfx_image_t* dst_image,
                                        scegfx_image_layout_t dst_image_layout,
                                        const scegfx_image_copy_t* region)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_image_vulkan_t* vk_src_image = (scegfx_image_vulkan_t*)src_image;
  scegfx_image_vulkan_t* vk_dst_image = (scegfx_image_vulkan_t*)dst_image;

  VkImageCopy vk_region = {
    .srcSubresource = *(VkImageSubresourceLayers*)&region->src_subresource,
    .srcOffset = *(VkOffset3D*)&region->src_offset,
    .dstSubresource = *(VkImageSubresourceLayers*)&region->dst_subresource,
    .dstOffset = *(VkOffset3D*)&region->dst_offset,
    .extent = *(VkExtent3D*)&region->extent,
  };

  vkCmdCopyImage(this->handle,
                 vk_src_image->handle,
                 (VkImageLayout)src_image_layout,
                 vk_dst_image->handle,
                 (VkImageLayout)dst_image_layout,
                 1,
                 &vk_region);
}

void
scegfx_command_buffer_vulkan_blit_image(scegfx_command_buffer_t* super,
                                        const scegfx_image_t* src_image,
                                        scegfx_image_layout_t src_image_layout,
                                        const scegfx_image_t* dst_image,
                                        scegfx_image_layout_t dst_image_layout,
                                        const scegfx_image_blit_t* region)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_image_vulkan_t* vk_src_image = (scegfx_image_vulkan_t*)src_image;
  scegfx_image_vulkan_t* vk_dst_image = (scegfx_image_vulkan_t*)dst_image;

  VkImageBlit vk_region = {
      .srcSubresource = *(VkImageSubresourceLayers*)&region->src_subresource,
      .srcOffsets = {
          [0] = *(VkOffset3D*)&region->src_offsets[0],
          [1] = *(VkOffset3D*)&region->src_offsets[1],
      },
      .dstSubresource = *(VkImageSubresourceLayers*)&region->dst_subresource,
      .dstOffsets = {
          [0] = *(VkOffset3D*)&region->dst_offsets[0],
          [1] = *(VkOffset3D*)&region->dst_offsets[1],
      },
  };

  vkCmdBlitImage(this->handle,
                 vk_src_image->handle,
                 (VkImageLayout)src_image_layout,
                 vk_dst_image->handle,
                 (VkImageLayout)dst_image_layout,
                 1,
                 &vk_region,
                 VK_FILTER_NEAREST);
}

void
scegfx_command_buffer_vulkan_copy_buffer_to_image(
  scegfx_command_buffer_t* super,
  const scegfx_buffer_t* buffer,
  const scegfx_image_t* image,
  scegfx_image_layout_t image_layout,
  const scegfx_buffer_image_copy_t* region)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_buffer_vulkan_t* vk_buffer = (scegfx_buffer_vulkan_t*)buffer;
  scegfx_image_vulkan_t* vk_image = (scegfx_image_vulkan_t*)image;

  VkBufferImageCopy vk_region = {
    .bufferOffset = region->buffer_offset,
    .bufferRowLength = region->buffer_row_length,
    .bufferImageHeight = region->buffer_image_height,
    .imageSubresource = *(VkImageSubresourceLayers*)&region->image_subresource,
    .imageOffset = *(VkOffset3D*)&region->image_offset,
    .imageExtent = *(VkExtent3D*)&region->image_extent,
  };

  vkCmdCopyBufferToImage(this->handle,
                         vk_buffer->handle,
                         vk_image->handle,
                         (VkImageLayout)image_layout,
                         1,
                         &vk_region);
}

void
scegfx_command_buffer_vulkan_copy_image_to_buffer(
  scegfx_command_buffer_t* super,
  const scegfx_image_t* image,
  scegfx_image_layout_t image_layout,
  const scegfx_buffer_t* buffer,
  const scegfx_buffer_image_copy_t* region)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_buffer_vulkan_t* vk_buffer = (scegfx_buffer_vulkan_t*)buffer;
  scegfx_image_vulkan_t* vk_image = (scegfx_image_vulkan_t*)image;

  VkBufferImageCopy vk_region = {
    .bufferOffset = region->buffer_offset,
    .bufferRowLength = region->buffer_row_length,
    .bufferImageHeight = region->buffer_image_height,
    .imageSubresource = *(VkImageSubresourceLayers*)&region->image_subresource,
    .imageOffset = *(VkOffset3D*)&region->image_offset,
    .imageExtent = *(VkExtent3D*)&region->image_extent,
  };

  vkCmdCopyImageToBuffer(this->handle,
                         vk_image->handle,
                         (VkImageLayout)image_layout,
                         vk_buffer->handle,
                         1,
                         &vk_region);
}

void
scegfx_command_buffer_vulkan_pipeline_barrier(
  scegfx_command_buffer_t* super,
  scegfx_pipeline_stage_t src_stage_mask,
  scegfx_pipeline_stage_t dst_stage_mask,
  uint32_t image_memory_barrier_count,
  const scegfx_image_memory_barrier_t* image_memory_barriers)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;

  assert(image_memory_barrier_count <= SCEGFX_VULKAN_MAX_BARRIER_COUNT);
  VkImageMemoryBarrier
    vk_image_memory_barriers[SCEGFX_VULKAN_MAX_BARRIER_COUNT];
  for (uint32_t i = 0; i < image_memory_barrier_count; ++i) {
    scegfx_image_vulkan_t* image =
      (scegfx_image_vulkan_t*)image_memory_barriers[i].image;
    vk_image_memory_barriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    vk_image_memory_barriers[i].srcAccessMask =
      image_memory_barriers[i].src_access_mask;
    vk_image_memory_barriers[i].dstAccessMask =
      image_memory_barriers[i].dst_access_mask;
    vk_image_memory_barriers[i].oldLayout =
      (VkImageLayout)image_memory_barriers[i].old_layout;
    vk_image_memory_barriers[i].newLayout =
      (VkImageLayout)image_memory_barriers[i].new_layout;
    vk_image_memory_barriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    vk_image_memory_barriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    vk_image_memory_barriers[i].image = image->handle;
    vk_image_memory_barriers[i].subresourceRange =
      *(VkImageSubresourceRange*)&image_memory_barriers[i].subresource_range;
  }
  vkCmdPipelineBarrier(this->handle,
                       src_stage_mask,
                       dst_stage_mask,
                       0,
                       0,
                       NULL,
                       0,
                       NULL,
                       image_memory_barrier_count,
                       vk_image_memory_barriers);
}

void
scegfx_command_buffer_vulkan_debug_marker_begin(
  scegfx_command_buffer_t* super,
  const scegfx_debug_marker_info_t* info)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  if (context->functions.CmdDebugMarkerBegin == NULL) {
    return;
  }

  VkDebugMarkerMarkerInfoEXT info_vk = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
    .pMarkerName = info->name,
    .color = { info->color[0], info->color[1], info->color[2], info->color[3] },
  };

  context->functions.CmdDebugMarkerBegin(this->handle, &info_vk);
}

void
scegfx_command_buffer_vulkan_debug_marker_end(scegfx_command_buffer_t* super)
{
  assert(super->initialized);
  scegfx_command_buffer_vulkan_t* this = (scegfx_command_buffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  if (context->functions.CmdDebugMarkerBegin == NULL) {
    return;
  }

  assert(context->functions.CmdDebugMarkerEnd);
  context->functions.CmdDebugMarkerEnd(this->handle);
}

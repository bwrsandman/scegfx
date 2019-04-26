//
// Created by sandy on 25/04/19.
//

#include "render_pass_vulkan.h"

#include <assert.h>

#include "context_vulkan.h"

bool
scegfx_render_pass_vulkan_initialize(scegfx_render_pass_t* super)
{
  assert(!super->initialized);
  scegfx_render_pass_vulkan_t* this = (scegfx_render_pass_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;

  VkAttachmentDescription color_attachment = {
    .format = context->surface_format.format,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentReference attachment_ref = {
    .attachment = 0,
    .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  VkSubpassDescription subpass = {
    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .colorAttachmentCount = 1,
    .pColorAttachments = &attachment_ref,
  };

  VkAttachmentDescription attachments[1] = {
    color_attachment,
  };

  VkRenderPassCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .attachmentCount = sizeof(attachments) / sizeof(attachments[0]),
    .pAttachments = attachments,
    .subpassCount = 1,
    .pSubpasses = &subpass,
  };
  vkCreateRenderPass(context->device, &info, NULL, &this->handle);

  this->super.initialized = true;
  return true;
}

void
scegfx_render_pass_vulkan_terminate(scegfx_render_pass_t* super)
{
  assert(super->initialized);
  scegfx_render_pass_vulkan_t* this = (scegfx_render_pass_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  vkDestroyRenderPass(context->device, this->handle, NULL);
  super->initialized = false;
}

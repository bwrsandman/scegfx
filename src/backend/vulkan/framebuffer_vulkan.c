//
// Created by sandy on 27/04/19.
//

#include "framebuffer_vulkan.h"

#include <assert.h>

#include <vulkan/vulkan.h>

#include "context_vulkan.h"
#include "image_view_vulkan.h"
#include "render_pass_vulkan.h"

bool
scegfx_framebuffer_vulkan_initialize(
  scegfx_framebuffer_t* super,
  const scegfx_framebuffer_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_framebuffer_vulkan_t* this = (scegfx_framebuffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  const scegfx_render_pass_vulkan_t* render_pass =
    (const scegfx_render_pass_vulkan_t*)info->render_pass;
  scegfx_image_view_vulkan_t* const* attachments =
    (scegfx_image_view_vulkan_t* const*)info->attachments;

  assert(info->attachment_count <=
         SCEGFX_VULKAN_MAX_FRAMEBUFFER_ATTACHMENT_COUNT);
  VkImageView vk_attachments[SCEGFX_VULKAN_MAX_FRAMEBUFFER_ATTACHMENT_COUNT];
  for (uint32_t i = 0; i < info->attachment_count; ++i) {
    assert(attachments[i]->super.initialized);
    vk_attachments[i] = attachments[i]->handle;
  }
  VkFramebufferCreateInfo vk_info = {
    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    .renderPass = render_pass->handle,
    .attachmentCount = info->attachment_count,
    .pAttachments = vk_attachments,
    .width = info->width,
    .height = info->height,
    .layers = 1,
  };
  VkResult result =
    vkCreateFramebuffer(context->device, &vk_info, NULL, &this->handle);

  if (result != VK_SUCCESS) {
    context->super.debug_callback(scegfx_debug_severity_error,
                                  __LINE__,
                                  FILE_BASENAME,
                                  "Unable to get create framebuffer");
    return false;
  }

  this->super.initialized = true;
  return true;
}

void
scegfx_framebuffer_vulkan_terminate(scegfx_framebuffer_t* super)
{
  assert(super->initialized);
  scegfx_framebuffer_vulkan_t* this = (scegfx_framebuffer_vulkan_t*)super;
  scegfx_context_vulkan_t* context = (scegfx_context_vulkan_t*)super->context;
  vkDestroyFramebuffer(context->device, this->handle, NULL);
  super->initialized = false;
}

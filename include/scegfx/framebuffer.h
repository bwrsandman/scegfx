//
// Created by sandy on 27/04/19.
//

#ifndef SCEGFX_FRAMEBUFFER_H
#define SCEGFX_FRAMEBUFFER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_image_view_t scegfx_image_view_t;
typedef struct scegfx_render_pass_t scegfx_render_pass_t;

typedef struct scegfx_framebuffer_t scegfx_framebuffer_t;

typedef struct scegfx_framebuffer_create_info_t
{
  const scegfx_render_pass_t* render_pass;
  uint32_t attachment_count;
  scegfx_image_view_t* const* attachments;
  uint32_t width;
  uint32_t height;
} scegfx_framebuffer_create_info_t;

typedef struct scegfx_framebuffer_api_vtable_t
{
  bool (*initialize)(scegfx_framebuffer_t* this,
                     const scegfx_framebuffer_create_info_t* info);
  void (*terminate)(scegfx_framebuffer_t* this);
} scegfx_framebuffer_api_vtable_t;

typedef struct scegfx_framebuffer_t
{
  bool initialized;
  const scegfx_framebuffer_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_framebuffer_t;

#endif // SCEGFX_FRAMEBUFFER_H

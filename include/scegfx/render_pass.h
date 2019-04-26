#ifndef SCEGFX_RENDER_PASS_H
#define SCEGFX_RENDER_PASS_H

#include <stdbool.h>

typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_render_pass_t scegfx_render_pass_t;

typedef struct scegfx_render_pass_api_vtable_t
{
  bool (*initialize)(scegfx_render_pass_t* this);
  void (*terminate)(scegfx_render_pass_t* this);
} scegfx_render_pass_api_vtable_t;

typedef struct scegfx_render_pass_t
{
  bool initialized;
  const scegfx_render_pass_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_render_pass_t;

#endif // SCEGFX_RENDER_PASS_H

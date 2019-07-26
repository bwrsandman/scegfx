//
// Created by sandy on 25/07/19.
//

#ifndef SCEGFX_SAMPLER_H
#define SCEGFX_SAMPLER_H

#include <stdbool.h>

typedef struct scegfx_sampler_t scegfx_sampler_t;
typedef struct scegfx_context_t scegfx_context_t;

typedef enum scegfx_filter_t
{
  scegfx_filter_nearest = 0,
  scegfx_filter_linear = 1,
} scegfx_filter_t;

typedef enum scegfx_sampler_mipmap_mode_t
{
  scegfx_sampler_mipmap_mode_nearest = 0,
  scegfx_sampler_mipmap_mode_linear = 1,
} scegfx_sampler_mipmap_mode_t;

typedef enum scegfx_sampler_address_mode_t
{
  scegfx_sampler_address_mode_repeat = 0,
  scegfx_sampler_address_mode_mirrored_repeat = 1,
  scegfx_sampler_address_mode_clamp_to_edge = 2,
  scegfx_sampler_address_mode_clamp_to_border = 3,
  scegfx_sampler_address_mode_mirror_clamp_to_edge = 4,
} scegfx_sampler_address_mode_t;

typedef struct scegfx_sampler_create_info_t
{
  scegfx_filter_t mag_filter;
  scegfx_filter_t min_filter;
  scegfx_sampler_mipmap_mode_t mipmap_mode;
  scegfx_sampler_address_mode_t address_mode_u;
  scegfx_sampler_address_mode_t address_mode_v;
  scegfx_sampler_address_mode_t address_mode_w;
} scegfx_sampler_create_info_t;

typedef struct scegfx_sampler_api_vtable_t
{
  bool (*initialize)(scegfx_sampler_t* this,
                     const scegfx_sampler_create_info_t* info);
  void (*terminate)(scegfx_sampler_t* this);
} scegfx_sampler_api_vtable_t;

typedef struct scegfx_sampler_t
{
  bool initialized;
  const scegfx_sampler_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
} scegfx_sampler_t;

#endif // SCEGFX_SAMPLER_H

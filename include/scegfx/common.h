//
// Created by sandy on 22/04/19.
//

#ifndef SCEGFX_COMMON_H
#define SCEGFX_COMMON_H

#include <stdint.h>

typedef uint64_t scegfx_device_size_t;

typedef enum scegfx_memory_properties_t
{
  scegfx_memory_properties_device_local = 0x0001u,
  scegfx_memory_properties_host_visible = 0x0002u,
  scegfx_memory_properties_host_coherent = 0x0004u,
  scegfx_memory_properties_host_cached = 0x0008u,
  scegfx_memory_properties_lazily_allocated = 0x0010u,
  scegfx_memory_properties_protected = 0x0020u,
} scegfx_memory_properties_t;

typedef struct scegfx_offset_2d_t
{
  int32_t x;
  int32_t y;
} scegfx_offset_2d_t;

typedef struct scegfx_offset_3d_t
{
  int32_t x;
  int32_t y;
  int32_t z;
} scegfx_offset_3d_t;

typedef struct scegfx_extent_2d_t
{
  uint32_t width;
  uint32_t height;
} scegfx_extent_2d_t;

typedef struct scegfx_extent_3d_t
{
  uint32_t width;
  uint32_t height;
  uint32_t depth;
} scegfx_extent_3d_t;

typedef struct scegfx_rect_2d_t
{
  scegfx_offset_2d_t offset;
  scegfx_extent_2d_t extent;
} scegfx_rect_2d_t;

#endif // SCEGFX_COMMON_H

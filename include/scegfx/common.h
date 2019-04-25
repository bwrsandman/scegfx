//
// Created by sandy on 22/04/19.
//

#ifndef SCEGFX_COMMON_H
#define SCEGFX_COMMON_H

#include <stdint.h>

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

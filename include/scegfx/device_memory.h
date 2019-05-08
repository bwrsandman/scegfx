//
// Created by sandy on 05/05/19.
//

#ifndef SCEGFX_MEMORY_H
#define SCEGFX_MEMORY_H

#include <stdbool.h>

#include "common.h"

typedef struct scegfx_device_memory_t
{
  bool initialized;
} scegfx_device_memory_t;

typedef struct scegfx_mapped_device_memory_range_t
{
  scegfx_device_memory_t* memory;
  scegfx_device_size_t offset;
  scegfx_device_size_t size;
} scegfx_mapped_device_memory_range_t;

typedef struct scegfx_device_memory_requirements_t
{
  scegfx_device_size_t size;
  scegfx_device_size_t alignment;
  uint32_t memory_type_bits;
} scegfx_device_memory_requirements_t;

typedef struct scegfx_device_memory_allocate_info_t
{
  scegfx_device_size_t allocation_size;
  uint32_t memory_type_index;
} scegfx_device_memory_allocate_info_t;

#endif // SCEGFX_MEMORY_H

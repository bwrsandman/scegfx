#ifndef SCEGFX_ALLOCATOR_H
#define SCEGFX_ALLOCATOR_H

#include "context.h"
#include <stdlib.h>

typedef struct scegfx_allocator_t
{
  void* (*allocator_callback)(void* ptr, size_t size, void* user_data);
  void* user_data;
} scegfx_allocator_t;

#endif // SCEGFX_ALLOCATOR_H

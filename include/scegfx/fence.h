//
// Created by sandy on 28/04/19.
//

#ifndef SCEGFX_FENCE_H
#define SCEGFX_FENCE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_fence_t scegfx_fence_t;

typedef struct scegfx_fence_api_vtable_t
{
  bool (*initialize)(scegfx_fence_t* this, bool signaled);
  void (*terminate)(scegfx_fence_t* this);

  bool (*wait)(scegfx_fence_t* fence, uint64_t timeout);
  bool (*reset)(scegfx_fence_t* fence);
} scegfx_fence_api_vtable_t;

typedef struct scegfx_fence_t
{
  bool initialized;
  const scegfx_fence_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
  const uint64_t max_wait_timeout;
} scegfx_fence_t;

#endif // SCEGFX_FENCE_H

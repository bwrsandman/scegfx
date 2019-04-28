//
// Created by sandy on 28/04/19.
//

#ifndef SCEGFX_SEMAPHORE_H
#define SCEGFX_SEMAPHORE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct scegfx_context_t scegfx_context_t;
typedef struct scegfx_semaphore_t scegfx_semaphore_t;

typedef struct scegfx_semaphore_api_vtable_t
{
  bool (*initialize)(scegfx_semaphore_t* this);
  void (*terminate)(scegfx_semaphore_t* this);
} scegfx_semaphore_api_vtable_t;

typedef struct scegfx_semaphore_t
{
  bool initialized;
  const scegfx_semaphore_api_vtable_t* api_vtable;
  const scegfx_context_t* context;
  const uint64_t max_wait_timeout;
} scegfx_semaphore_t;

#endif // SCEGFX_SEMAPHORE_H

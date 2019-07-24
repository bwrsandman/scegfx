//
// Created by sandy on 23/07/19.
//

#ifndef SCEGFX_VAO_MAP_H
#define SCEGFX_VAO_MAP_H

#include <scegfx/common.h>
#include <stdbool.h>
#include <stdint.h>

enum
{
  SCEGFX_VAO_MAP_VERTEX_ATTRIBUTE_COMMAND_COUNT = 2,
  SCEGFX_VAO_MAP_BUFFER_SIZE = 0x20,
};

typedef struct scegfx_vao_map_key_t
{
  struct
  {
    uint8_t handle;
    scegfx_device_size_t offset;
  } index;
  struct
  {
    uint8_t handle;
    scegfx_device_size_t offset;
    scegfx_device_size_t stride;
  } vertex;
  uint8_t attribute_count;
  struct
  {
    uint8_t binding;
    uint8_t location;
    uint32_t size;
    uint32_t format;
    bool normalized;
    uint32_t offset;
  } attributes[SCEGFX_VAO_MAP_VERTEX_ATTRIBUTE_COMMAND_COUNT];
} scegfx_vao_map_key_t;

bool
scegfx_vao_map_key_equals(const scegfx_vao_map_key_t* lhs,
                          const scegfx_vao_map_key_t* rhs);

typedef struct scegfx_vao_map_bucket_t
{
  scegfx_vao_map_key_t key;
  uint8_t vao;
} scegfx_vao_map_bucket_t;

typedef struct scegfx_vao_map_t
{
  scegfx_vao_map_bucket_t buffer[SCEGFX_VAO_MAP_BUFFER_SIZE];
} scegfx_vao_map_t;

bool
scegfx_vao_map_get(const scegfx_vao_map_t* map,
                   const scegfx_vao_map_key_t* key,
                   uint8_t* vao);
void
scegfx_vao_map_add(scegfx_vao_map_t* map,
                   const scegfx_vao_map_key_t* key,
                   uint8_t vao);

#endif // SCEGFX_VAO_MAP_H

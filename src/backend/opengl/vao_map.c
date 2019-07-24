//
// Created by sandy on 23/07/19.
//

#include "vao_map.h"

#include <assert.h>

bool
scegfx_vao_map_key_equals(const scegfx_vao_map_key_t* lhs,
                          const scegfx_vao_map_key_t* rhs)
{
  if (lhs->index.handle != rhs->index.handle) {
    return false;
  }
  if (lhs->index.offset != rhs->index.offset) {
    return false;
  }
  if (lhs->vertex.handle != rhs->vertex.handle) {
    return false;
  }
  if (lhs->vertex.offset != rhs->vertex.offset) {
    return false;
  }
  if (lhs->vertex.stride != rhs->vertex.stride) {
    return false;
  }
  if (lhs->attribute_count != rhs->attribute_count) {
    return false;
  }
  assert(lhs->attribute_count <=
         sizeof(lhs->attributes) / sizeof(lhs->attributes[0]));
  for (uint32_t i = 0; i < lhs->attribute_count; ++i) {
    if (lhs->attributes[i].binding != rhs->attributes[i].binding) {
      return false;
    }
    if (lhs->attributes[i].location != rhs->attributes[i].location) {
      return false;
    }
    if (lhs->attributes[i].size != rhs->attributes[i].size) {
      return false;
    }
    if (lhs->attributes[i].format != rhs->attributes[i].format) {
      return false;
    }
    if (lhs->attributes[i].normalized != rhs->attributes[i].normalized) {
      return false;
    }
    if (lhs->attributes[i].offset != rhs->attributes[i].offset) {
      return false;
    }
  }
  return true;
}

uint32_t
scegfx_vao_map_hash_key(const scegfx_vao_map_key_t* key, uint32_t map_size)
{
  uint64_t hash = 0;
  hash += key->index.handle;
  hash += key->index.offset << 1u;
  hash += key->vertex.handle << 2u;
  hash += key->vertex.offset << 3u;
  hash += key->vertex.stride << 4u;
  for (uint32_t i = 0; i < key->attribute_count; ++i) {
    hash += (i + 1u) * key->attributes[i].binding << 5u;
    hash += (i + 1u) * key->attributes[i].location << 6u;
    hash += (i + 1u) * key->attributes[i].size << 7u;
    hash += (i + 1u) * key->attributes[i].format << 8u;
    hash += (i + 1u) * key->attributes[i].offset << 9u;
  }
  return hash % map_size;
}

bool
scegfx_vao_map_get(const scegfx_vao_map_t* map, const scegfx_vao_map_key_t* key, uint8_t* vao)
{
  uint32_t map_size = SCEGFX_VAO_MAP_BUFFER_SIZE;
  uint32_t hash = scegfx_vao_map_hash_key(key, map_size);
  const scegfx_vao_map_bucket_t* result;
  for (uint32_t i = 0; i < map_size; ++i) {
    result = &map->buffer[hash++];
    // Does not exist
    if (result->vao == 0) {
      return false;
    }
    // No collision
    if (scegfx_vao_map_key_equals(&result->key, key)) {
      *vao = result->vao;
      return true;
    }
  }
  return false;
}

void
scegfx_vao_map_add(scegfx_vao_map_t* map,
                   const scegfx_vao_map_key_t* key,
                   uint8_t vao)
{
  assert(vao > 0);
  uint32_t map_size = SCEGFX_VAO_MAP_BUFFER_SIZE;
  uint32_t hash = scegfx_vao_map_hash_key(key, map_size);
  scegfx_vao_map_bucket_t* result;
  for (uint32_t i = 0; i < map_size; ++i) {
    result = &map->buffer[hash];
    // No collision
    if (result->vao == 0 || scegfx_vao_map_key_equals(&result->key, key)) {
      result->vao = vao;
      result->key = *key;
      return;
    }
    hash++;
  }
  // Map is filled
  assert(false);
}

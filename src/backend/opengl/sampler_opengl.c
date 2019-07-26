//
// Created by sandy on 26/07/19.
//

#include "sampler_opengl.h"

#include <assert.h>

bool
scegfx_sampler_opengl_initialize(scegfx_sampler_t* super,
                                 const scegfx_sampler_create_info_t* info)
{
  assert(!super->initialized);
  scegfx_sampler_opengl_t* this = (scegfx_sampler_opengl_t*)super;

  this->mag_filter = /*GL_NEAREST,GL_LINEAR*/ 0x2600 + info->mag_filter;
  if (/* !is mipped mapped */ true) {
    this->min_filter = /*GL_NEAREST,GL_LINEAR*/ 0x2600 + info->min_filter;
  } else if (/*minFilter of VK_FILTER_LINEAR and mipmapMode of
                VK_SAMPLER_MIPMAP_MODE_NEAREST*/
             false) {
    this->min_filter = /*GL_LINEAR_MIPMAP_NEAREST*/ 0x2701;
  } else if (/*minFilter of VK_FILTER_LINEAR and mipmapMode of
                VK_SAMPLER_MIPMAP_MODE_LINEAR*/
             false) {
    this->min_filter = /*GL_LINEAR_MIPMAP_LINEAR*/ 0x2703;
  } else if (/*minFilter of VK_FILTER_NEAREST and mipmapMode of
                VK_SAMPLER_MIPMAP_MODE_LINEAR*/
             false) {
    this->min_filter = /*GL_NEAREST_MIPMAP_LINEAR*/ 0x2702;
  } else if (/*minFilter of VK_FILTER_NEAREST and mipmapMode of
                VK_SAMPLER_MIPMAP_MODE_NEAREST*/
             false) {
    this->min_filter = /*GL_NEAREST_MIPMAP_NEAREST*/ 0x2700;
  }
  int32_t* lhs = &this->texture_wrap_s;
  const scegfx_sampler_address_mode_t* rhs = &info->address_mode_u;
  for (uint8_t i = 0; i < 3; ++i) {
    switch (rhs[i]) {
      case scegfx_sampler_address_mode_repeat:
        lhs[i] = /*GL_REPEAT*/ 0x2901;
        break;
      case scegfx_sampler_address_mode_mirrored_repeat:
        lhs[i] = /*GL_MIRRORED_REPEAT*/ 0x8370;
        break;
      case scegfx_sampler_address_mode_clamp_to_edge:
        lhs[i] = /*GL_CLAMP_TO_EDGE*/ 0x812F;
        break;
      case scegfx_sampler_address_mode_clamp_to_border:
        lhs[i] = /*GL_CLAMP_TO_BORDER*/ 0x812D;
        break;
      case scegfx_sampler_address_mode_mirror_clamp_to_edge:
        lhs[i] = /*GL_MIRROR_CLAMP_TO_EDGE*/ 0x8743;
        break;
    }
  }

  super->initialized = true;
  return true;
}

void
scegfx_sampler_opengl_terminate(scegfx_sampler_t* super)
{
  assert(super->initialized);
}

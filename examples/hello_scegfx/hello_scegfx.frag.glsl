#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : enable

#include "bridging_header.h"

layout(location = F_UV_LOCATION) in vec2 f_uv;
layout(binding = F_IMAGE_SAMPLER_LOCATION) uniform sampler2D texSampler;
layout(location = FB_COLOR_LOCATION) out vec4 fb_color;

void main()
{
    fb_color = texture(texSampler, f_uv);
}

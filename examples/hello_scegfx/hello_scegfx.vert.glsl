#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : enable

#include "bridging_header.h"

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = V_POSITION_LOCATION) in vec4 v_position;
layout(location = V_COLOR_LOCATION) in vec4 v_color;

layout(location = F_COLOR_LOCATION) out vec4 f_color;

void main()
{
    gl_Position = v_position;
    f_color = v_color;
}

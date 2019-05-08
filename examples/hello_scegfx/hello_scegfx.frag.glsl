#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec4 f_color;
layout(location = 0) out vec4 fb_color;

void main()
{
    fb_color = f_color;
}

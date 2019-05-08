#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

out gl_PerVertex
{
    vec4 gl_Position;
};

struct vertex_t {
    vec4 position;
    vec4 color;
};

layout (location = 0) out vec4 f_color;

vertex_t vertices[3] = {
    { vec4(0.0, -1.0f, 0.0f, 1.0f), vec4(0.0, 1.0f, 1.0f, 1.0f), },
    { vec4(-1.0, 1.0f, 0.0f, 1.0f), vec4(1.0, 0.0f, 1.0f, 1.0f), },
    { vec4(1.0, 1.0f, 0.0f, 1.0f), vec4(1.0, 1.0f, 0.0f, 1.0f), },
};

void main()
{
    gl_Position = vertices[gl_VertexIndex].position;
    f_color = vertices[gl_VertexIndex].color;
}

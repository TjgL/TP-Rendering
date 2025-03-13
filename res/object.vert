#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
uniform mat4 view_projection_matrix;

out vec2 uv;
out vec3 normals;

out vec3 vertex_position;

void main()
{
    uv = in_uv;
    vertex_position = in_position;
    normals = in_normal;
    gl_Position = view_projection_matrix * vec4(in_position, 1.);
}
#version 410

layout(location = 0) in vec3 in_position;
uniform mat4 view_projection_matrix;

in vec2 in_uv;
out vec2 uv;

out vec3 vertex_position;

void main()
{
    uv = in_uv;
    vertex_position = in_position;
    gl_Position = view_projection_matrix * vec4(in_position, 1.);
}
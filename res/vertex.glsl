#version 410

layout(location = 0) in vec2 in_position;
uniform mat4 view_projection_matrix;

void main()
{
    vec2 position = in_position;
    gl_Position = view_projection_matrix * vec4(position, 0., 1.);
}
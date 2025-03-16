#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
uniform mat4 view_projection_matrix;
uniform mat4 normal_matrix;

uniform vec3 point_light_pos;
out vec3 point_light_dir;

out vec2 uv;
out vec3 normals;
out vec3 vertex_position;

vec3 apply_matrix_to_position(mat4 matrix, vec3 point);

void main()
{
    uv = in_uv;
    vertex_position = apply_matrix_to_position(view_projection_matrix, in_position);
    normals = normalize((normal_matrix * vec4(in_position, 0.f)).xyz);

    point_light_dir = in_position - point_light_pos;

    gl_Position = view_projection_matrix * vec4(in_position, 1.);
}

vec3 apply_matrix_to_position(mat4 matrix, vec3 point)
{
    vec4 tmp = matrix * vec4(point, 1.);
    return tmp.xyz / tmp.w;
}
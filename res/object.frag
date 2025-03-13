#version 410

uniform sampler2D my_texture;
in vec2 uv;
in vec3 normals;

out vec4 out_color;

void main()
{
    vec4 texture_color = texture(my_texture, uv);
    out_color = vec4(normals.xyz, 1.);
}
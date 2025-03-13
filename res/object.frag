#version 410


struct DirLight {
    vec3 direction;

    vec3 color;
    vec3 ambient;
};
uniform DirLight directional_light;

uniform sampler2D my_texture;

in vec2 uv;
in vec3 normals;
in vec3 vertex_position;

out vec4 out_color;

vec3 CalcDirLight(DirLight light, vec3 normal);

void main()
{
    vec3 norm = normalize(normals);
    vec4 texture_color = texture(my_texture, uv);

    vec3 directional = CalcDirLight(directional_light, norm);

    vec3 result = directional * texture_color.xyz;

    out_color = vec4(result.xyz, 1.);
}

vec3 CalcDirLight(DirLight light, vec3 normal) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 ambient = light.ambient;
    vec3 color = light.color * diff;
    return (ambient + color);
}

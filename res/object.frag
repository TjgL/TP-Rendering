#version 410

struct PointLight {
    vec3 position;
    vec3 diffuse;
};
uniform PointLight point_light;

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

vec3 calc_dir_light(DirLight light, vec3 normal);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos);

void main()
{
    vec4 texture_color = texture(my_texture, uv);

    vec3 directional = calc_dir_light(directional_light, normals);
    vec3 point = calc_point_light(point_light, normals, vertex_position);

    vec3 result = (directional + point) * texture_color.xyz;

    out_color = vec4(result.xyz, 1.);
}

vec3 calc_dir_light(DirLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 ambient = light.ambient;
    vec3 color = light.color * diff;
    return (ambient + color);
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float intensity = max(dot(normal, lightDir), 0.0);
    float distance = length(light.position - fragPos);
    float attenuation = intensity / (distance * distance);

    vec3 diffuse = light.diffuse * attenuation;
    return diffuse;
}

#version 410

uniform sampler2D my_texture;

uniform vec3 light_direction;
uniform vec3 light_color;
uniform float light_intensity;

uniform vec3 ambient_color;
uniform float ambient_intensity;

in vec2 uv;
in vec3 normals;
in vec3 vertex_position;

out vec4 out_color;

void main()
{
    vec4 texture_color = texture(my_texture, uv);

    vec3 ambient = ambient_intensity * ambient_color;

    vec3 norm = normalize(normals);
    vec3 lightDir = normalize(light_direction - vertex_position);
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = diff * light_color * light_intensity;

    vec3 result = (ambient + diffuse) * texture_color.xyz;

    out_color = vec4(result.xyz, 1.);
}
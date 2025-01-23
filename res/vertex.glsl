#version 410

layout(location = 0) in vec2 in_position;
uniform float aspect_ratio;
uniform float time;

void main()
{
    vec2 position = in_position;

    position.x = position.x / aspect_ratio;

    position.x += sin(time) / 2;
    position.y += cos(time) / 2;

    gl_Position = vec4(position, 0., 1.);
}
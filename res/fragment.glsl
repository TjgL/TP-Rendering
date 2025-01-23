#version 410

out vec4 out_color;
uniform vec4 color;
uniform float time;
uniform float dtime;

void main()
{
    out_color = color;
}
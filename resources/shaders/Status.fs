#version 460 core
in vec2 UV;
out vec4 FragColor;

uniform vec4 color;
uniform sampler2D diffuse;

void main()
{
    FragColor = texture(diffuse, UV) * color;
}
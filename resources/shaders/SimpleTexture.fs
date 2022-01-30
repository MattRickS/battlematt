#version 460 core
in vec2 UV;
out vec4 FragColor;

uniform vec4 color = vec4(1.0);
uniform sampler2D diffuse;

void main()
{
    FragColor = vec4(vec3(texture(diffuse, UV)), 1.0) * color;
}
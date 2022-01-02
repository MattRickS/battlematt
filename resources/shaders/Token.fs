#version 460 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec4 borderColor;
uniform float borderWidth;

void main()
{
    vec2 offset = UV - 0.5;

    float dist = dot(offset, offset);
    if (dist <= 0.25 - borderWidth * 0.25)
        FragColor = vec4(vec3(texture(diffuse, UV)), 1.0);
    else if (dist < 0.25)
        FragColor = borderColor;
    else
        FragColor = vec4(0);
}
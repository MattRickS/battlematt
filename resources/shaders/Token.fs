#version 460 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec4 borderColor;
uniform float borderWidth;
uniform vec4 highlightColor;

float highlightWidth = 0.04;
float highlightBand = 0.25 - highlightWidth;

void main()
{
    vec2 offset = UV - 0.5;

    float dist = dot(offset, offset);
    if (dist <= highlightBand - borderWidth * highlightBand)
        FragColor = vec4(vec3(texture(diffuse, UV)), 1.0);
    else if (dist < highlightBand)
        FragColor = borderColor;
    else if (highlightColor.w > 0 && dist < 0.25)
    {
        float alpha = (0.25 - dist) * (1 / highlightWidth);
        FragColor = vec4(vec3(highlightColor), alpha * alpha);
    }
    else
        FragColor = vec4(0);
}
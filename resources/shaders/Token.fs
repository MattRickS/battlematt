#version 460 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec4 borderColor;
uniform float borderWidth;
// Highlight Color will have no alpha if not highlighted
uniform vec4 highlightColor;
uniform vec4 color;

float highlightWidth = 0.06;
float maxBand = 0.25;
float highlightBand = maxBand - highlightWidth;
float borderBand = highlightBand * (1.0 - borderWidth);
float AAsize = 0.001;

void main()
{
    vec2 offset = UV - 0.5;

    float dist = dot(offset, offset);
    if (dist <= borderBand)
    {
        FragColor = vec4(vec3(texture(diffuse, UV)) * (1 - dist * 3), 1.0) * color.w;
        if (dist >= borderBand - AAsize)
            FragColor = mix(FragColor, borderColor, (dist - (borderBand - AAsize)) / AAsize) * color.w;
    }
    else if (dist < highlightBand)
    {
        FragColor = borderColor * color.w;
        if (dist >= highlightBand - AAsize)
            FragColor = mix(borderColor, highlightColor.w > 0 ? highlightColor : vec4(0), (dist - (highlightBand - AAsize)) / AAsize) * color.w;
    }
    else if (highlightColor.w > 0 && dist < maxBand)
    {
        float alpha = (maxBand - dist) * (1 / highlightWidth);
        FragColor = vec4(vec3(highlightColor), alpha) * color.w;
    }
    else
        FragColor = vec4(0);
}
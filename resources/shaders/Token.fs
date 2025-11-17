#version 460 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec4 borderColor;
// Border Width is 0.0 to 1.0, fraction of radius
uniform float borderWidth;
// Highlight Color will have no alpha if not highlighted
uniform vec4 highlightColor;
uniform float opacity;

float highlightWidth = 0.06;
// Using dot distance, so this is 0.5*0.5, ie, circular image
float maxBand = 0.25;
float highlightBand = maxBand - highlightWidth;
float borderBand = highlightBand * (1.0 - borderWidth);
float AAsize = 0.001;
// Convert back to linear space and invert to get the UV scaling factor to fit inside the border
float fractionalImageSize = 1.0 / ( sqrt(borderBand) / sqrt(maxBand));

void main()
{
    vec2 offset = UV - 0.5;

    float dist = dot(offset, offset);
    // Inner image
    if (dist <= borderBand)
    {
        FragColor = vec4(vec3(texture(diffuse, 0.5 + offset * fractionalImageSize)), 1.0) * opacity;
        if (dist >= borderBand - AAsize)
            FragColor = mix(FragColor, borderColor, (dist - (borderBand - AAsize)) / AAsize) * opacity;
    }
    // Border
    else if (dist < highlightBand)
    {
        FragColor = borderColor * opacity;
        if (dist >= highlightBand - AAsize)
            FragColor = mix(borderColor, highlightColor.w > 0 ? highlightColor : vec4(0), (dist - (highlightBand - AAsize)) / AAsize) * opacity;
    }
    // Selection Highlight
    else if (highlightColor.w > 0 && dist < maxBand)
    {
        float alpha = (maxBand - dist) * (1 / highlightWidth);
        FragColor = vec4(vec3(highlightColor), alpha) * opacity;
    }
    else
        FragColor = vec4(0);
}
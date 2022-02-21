#version 460 core
in vec2 UV;
out vec4 FragColor;

uniform vec4 color = vec4(1.0);
uniform sampler2D diffuse;
uniform bool useGreyscale = false;

// Rec709
vec3 greyscale(vec4 col)
{
    return vec3(col.r * 0.2126 + col.g * 0.7152 + col.b * 0.0722);
}

void main()
{
    FragColor = vec4(vec3(texture(diffuse, UV)), 1.0) * color;
    if (useGreyscale)
        FragColor.rgb = greyscale(FragColor);
}
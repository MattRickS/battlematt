#version 460 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec4 borderColor;

ivec2 texSize;
vec2 offset;

void main()
{
    // Use the texture dimensions to ensure we always draw a perfect circle
    // This can be gotten rid of once the Token class correctly aligns the UVs
    texSize = textureSize(diffuse, 0);
    if (texSize.x < texSize.y)
        offset = vec2(UV.x - 0.5, (UV.y - 0.5) * (float(texSize.y) / float(texSize.x)));
    else if (texSize.y > texSize.x)
        offset = vec2((UV.x - 0.5) * float(texSize.x) / float(texSize.y), UV.y - 0.5);
    else
        offset = UV - 0.5;

    float dist = dot(offset, offset);
    if (dist <= 0.2)
        FragColor = vec4(vec3(texture(diffuse, UV)), 1.0);
    else if (dist < 0.25)
        FragColor = borderColor;
    else
        FragColor = vec4(0);
}
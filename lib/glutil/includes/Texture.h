#pragma once
#include <glad/glad.h>
#include <stb_image.h>


class Texture
{
public:
    const char *filename;
    int width, height, numChannels;
    GLuint ID;

    Texture() {}
    Texture(const char *filename);

    void activate(GLuint textureID) const;
};

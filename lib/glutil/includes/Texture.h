#pragma once
#include <string>

#include <glad/glad.h>
#include <stb_image.h>


class Texture
{
public:
    std::string filename;
    int width = 0, height = 0, numChannels = 0;
    GLuint ID;

    Texture() {}
    Texture(const char *filename);

    void activate(GLuint textureID) const;
};

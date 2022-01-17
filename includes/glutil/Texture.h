#pragma once
#include <string>

#include <glad/glad.h>
#include <stb_image.h>


class Texture
{
public:
    std::string filename;
    int width = 0, height = 0, numChannels = 0;
    GLuint ID = 0;

    Texture() {}
    Texture(const char *filename);

    void activate(GLuint textureID) const;
    bool IsValid() const;
    std::string Name() const;
};

#include <filesystem>
#include <iostream>

#include <stb_image.h>
#include <glad/glad.h>

#include <glutil/Texture.h>


Texture::Texture(const char *filename) : filename(filename)
{
    // Load data into the texture
    unsigned char* data = stbi_load(filename, &width, &height, &numChannels, 0);
    if (data)
    {
        glGenTextures(1, &ID);

        GLenum format;
        if (numChannels == 1)
            format = GL_RED;
        else if (numChannels == 3)
            format = GL_RGB;
        else if (numChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, ID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping parameters
        // TODO: Might want different modes, eg, GL_CLAMP_TO_EDGE
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cerr << "Loaded: " << filename << " as ID " << ID << std::endl;
    }
    else
        std::cerr << "Failed to load texture: " << filename << std::endl;
    
    // Cleanup
    stbi_image_free(data);

}

void Texture::activate(GLuint textureID) const
{
    glActiveTexture(textureID);
    glBindTexture(GL_TEXTURE_2D, ID);
}

bool Texture::IsValid() const { return ID > 0; }

std::string Texture::Name() const { return std::filesystem::path(filename).stem(); }

#pragma once
#include <unordered_map>
#include <string>

#include <Texture.h>


class TextureCache
{
public:
    static bool HasTexture(const std::string path);
    static void LoadTexture(const std::string path);
    static void UnloadTexture(const std::string path);
    static void Clear();
    static Texture& GetTexture(const std::string path);
    static unsigned int NumTextures();

private:
    static std::unordered_map<std::string, Texture> loadedTextures;
    TextureCache() {}
};

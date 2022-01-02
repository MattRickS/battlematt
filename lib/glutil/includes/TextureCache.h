#pragma once
#include <unordered_map>
#include <string>

#include <Texture.hpp>


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


std::unordered_map<std::string, Texture> TextureCache::loadedTextures;


bool TextureCache::HasTexture(const std::string path)
{
    return loadedTextures.find(path) != loadedTextures.end();
}

void TextureCache::LoadTexture(const std::string path)
{
    loadedTextures[path] = Texture(path.c_str());
}

void TextureCache::UnloadTexture(const std::string path)
{
    glDeleteTextures(1, &loadedTextures[path].ID);
    loadedTextures.erase(path);
}

void TextureCache::Clear()
{
    for(auto& it: loadedTextures)
    {
        glDeleteTextures(1, &it.second.ID);
    }
    loadedTextures.clear();
}

Texture& TextureCache::GetTexture(const std::string path)
{
    if (!HasTexture(path))
        LoadTexture(path);
    return loadedTextures[path];
}

unsigned int TextureCache::NumTextures()
{
    return loadedTextures.size();
}

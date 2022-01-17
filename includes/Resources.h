#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <glutil/Texture.h>


class Resources
{
public:
    enum MeshType { Quad, Quad2 };
    enum ShaderType { Grid, Image, ScreenRect, Token };
    enum TextureType { Default };

    Resources() {}

    void CreateMesh(MeshType meshType, std::vector<Vertex> vertices, std::vector<uint> indices);
    std::shared_ptr<Mesh> GetMesh(MeshType meshType);
    void CreateShader(ShaderType shaderType, const char* vs, const char* fs);
    std::shared_ptr<Shader> GetShader(ShaderType shaderType);
    void CreateTexture(TextureType textureType, std::string path);
    std::shared_ptr<Texture> GetTexture(TextureType textureType);
    std::shared_ptr<Texture> GetTexture(std::string path);

private:
    std::unordered_map<MeshType, std::shared_ptr<Mesh>> m_meshes;
    std::unordered_map<ShaderType, std::shared_ptr<Shader>> m_shaders;
    std::unordered_map<TextureType, std::shared_ptr<Texture>> m_textureTypes;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
};

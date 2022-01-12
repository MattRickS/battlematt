#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include <Mesh.h>
#include <Shader.h>
#include <Texture.h>


class Resources
{
public:
    enum MeshType { Quad, Quad2 };
    enum ShaderType { Grid, Image, ScreenRect, Token };

    Resources() {}

    void CreateMesh(MeshType meshType, std::vector<Vertex> vertices, std::vector<uint> indices);
    std::shared_ptr<Mesh> GetMesh(MeshType meshType);
    void CreateShader(ShaderType ShaderType, const char* vs, const char* fs);
    std::shared_ptr<Shader> GetShader(ShaderType shaderType);
    std::shared_ptr<Texture> GetTexture(std::string path);

private:
    std::unordered_map<MeshType, std::shared_ptr<Mesh>> m_meshes;
    std::unordered_map<ShaderType, std::shared_ptr<Shader>> m_shaders;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
};

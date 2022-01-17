#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>

#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <glutil/Texture.h>

#include <Resources.h>


void Resources::CreateMesh(MeshType meshType, std::vector<Vertex> vertices, std::vector<uint> indices)
{
    m_meshes[meshType] = std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Resources::GetMesh(MeshType meshType)
{
    return m_meshes.at(meshType);
}

void Resources::CreateShader(ShaderType shaderType, const char* vs, const char* fs)
{
    m_shaders[shaderType] = std::make_shared<Shader>(vs, fs);
}

std::shared_ptr<Shader> Resources::GetShader(ShaderType shaderType)
{
    return m_shaders.at(shaderType);
}

void Resources::CreateTexture(TextureType textureType, std::string path)
{
    m_textureTypes[textureType] = GetTexture(path);
}

std::shared_ptr<Texture> Resources::GetTexture(TextureType textureType)
{
    return m_textureTypes.at(textureType);
}

std::shared_ptr<Texture> Resources::GetTexture(std::string path)
{
    auto [it, success] = m_textures.try_emplace(path, nullptr);
    if (success)
    {
        std::cerr << "Loading texture: " << path  << std::endl;
        it->second = std::make_shared<Texture>(path.c_str());
    }
    else
        std::cerr << "Re-using texture: " << path  << std::endl;
    return it->second;
}

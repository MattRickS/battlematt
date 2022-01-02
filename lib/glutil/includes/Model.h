#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Mesh.hpp>
#include <Shader.hpp>
#include <TextureCache.hpp>


class Model
{
public:
    Model(const char *path);
    void Draw(Shader &shader);
private:
    std::vector<Mesh> meshes;
    std::filesystem::path directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::string getTexturePath(const char *texturePath) const;
    void loadMaterialTextures(std::vector<TextureID> &textures, aiMaterial *mat, aiTextureType type, TextureType textureType);
};


Model::Model(const char *path)
{
    loadModel(path);
}


void Model::Draw(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    // Common options
    // aiProcess_Triangulate: Forces all mesh data into triangles
    // aiProcess_FlipUVs: Flips UV coords on the Y-axis to match OpenGL
    // aiProcess_GenNormals: Creates normal vectors (if not existing)
    // aiProcess_SplitLargeMeshes: Splits large meshes into smaller meshes
    // aiProcess_OptimizeMeshes: Joins smaller meshes into one for less draw calls
    // Full list: http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = std::filesystem::path(path.substr(0, path.find_last_of("/")));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureID> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        
        loadMaterialTextures(textures, material, aiTextureType_DIFFUSE, TextureType::diffuse);
        loadMaterialTextures(textures, material, aiTextureType_SPECULAR, TextureType::specular);
    }

    return Mesh(vertices, indices, textures);
}

void Model::loadMaterialTextures(std::vector<TextureID> &textures, aiMaterial *mat, aiTextureType type, TextureType textureType)
{
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string path = getTexturePath(str.C_Str());

        if(!TextureCache::HasTexture(path))
        {
            TextureCache::LoadTexture(path);
        }
        
        TextureID textureID {TextureCache::GetTexture(path), textureType};
        textures.push_back(textureID);
    }
}

std::string Model::getTexturePath(const char *texturePath) const
{
    std::filesystem::path path(texturePath);
    if (path.is_absolute())
        return path;
    return directory / texturePath;
}


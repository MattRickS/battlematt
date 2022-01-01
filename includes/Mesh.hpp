#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <Shader.hpp>
#include <Texture.hpp>


struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

enum TextureType
{
    diffuse,
    specular
};

struct TextureID
{
    Texture &texture;
    TextureType type;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureID> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureID> textures);
    void Draw(Shader &shader);

private:
    GLuint VAO, VBO, EBO;
    void setupMesh();
};



Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureID> textures) : vertices(vertices), indices(indices), textures(textures)
{
    setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) : vertices(vertices), indices(indices)
{
    setupMesh();
}

void Mesh::Draw(Shader &shader)
{
    // TODO: Assess alternative options
    // Bind textures by assuming shader sampler naming convention of material.texture_{type}{N}
    unsigned int diffuseNum = 1;
    unsigned int specularNum = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string name;
        TextureType texureType = textures[i].type;
        if (texureType == TextureType::diffuse)
            name = "material.diffuse" + std::to_string(diffuseNum++);
        else if (texureType == TextureType::specular)
            name = "material.specular" + std::to_string(specularNum++);
        else
            throw "Unknown texture type";
        
        shader.setInt(name.c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].texture.ID);
    }
    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // Texture Coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    /*
    // Alternative is to not use the Vertex struct but store the model's positions/normals/etc...
    // directly, reserve a buffer for the combined size (pass NULL for the data) and use
    // glBufferSubData to batch load in sections. Interleaved data is still preferred as it keeps
    // all properties for each vertex together in memory which is more efficient for shaders.

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(normals), &normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(normals), sizeof(tex), &tex);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(positions)));  
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(positions) + sizeof(normals)));
    */

	glBindVertexArray(0);
}

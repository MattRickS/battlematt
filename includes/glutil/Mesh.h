#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <glutil/Shader.h>


struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    void Draw(Shader &shader);

private:
    GLuint VAO, VBO, EBO;
    void setupMesh();
};

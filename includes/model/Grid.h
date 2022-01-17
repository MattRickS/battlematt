#pragma once
#include <glm/glm.hpp>

#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <model/Shape2D.h>
#include <model/Token.h>


enum ShapeGridSize
{
    Fine = -4,
    Diminutive = -3,
    Tiny = -2,
    Small = -1,
    Medium = 0,
    Large = 1,
    Huge = 2,
    Gargantuan = 3,
    Colossal = 4
};


class Grid
{
public:
    Grid(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader);

    void Draw();
    void SetScale(float scale);
    float GetScale();
    void SetColour(glm::vec3 colour);
    glm::vec3 GetColour();
    glm::vec2 NearestCenter(float tokenSize, glm::vec2 pos);
    glm::vec2 NearestCorner(float tokenSize, glm::vec2 pos);
    ShapeGridSize GetShapeGridSize(std::shared_ptr<Shape2D> token);
    float SnapGridSize(ShapeGridSize tokenGridSize);
    float SnapGridSize(float size);
    glm::vec2 ShapeSnapPosition(std::shared_ptr<Shape2D> token, glm::vec2 pos);

private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
    float m_scale = 1.0f;
    glm::vec3 m_colour = glm::vec3(0.2);
};

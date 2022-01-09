#include <glm/glm.hpp>

#include <Primitives.h>
#include <Shader.h>
#include <Token.h>
#include <Grid.h>


// Default quad is (-0.5 -> 0.5), we want (-1 -> 1) to fill the screen
Grid::Grid(): shader("resources/shaders/Grid.vs", "resources/shaders/Grid.fs"), mesh(2)
{
    shader.use();
    shader.setFloat("gridScale", m_scale);
    shader.setFloat3("gridColour", m_colour.x, m_colour.y, m_colour.z);
}

void Grid::Draw()
{
    shader.use();
    mesh.Draw(shader);
}

void Grid::SetScale(float scale)
{
    m_scale = scale;
    shader.use();
    shader.setFloat("gridScale", m_scale);
}

float Grid::GetScale() { return m_scale; }

void Grid::SetColour(glm::vec3 colour)
{
    m_colour = colour;
    shader.use();
    shader.setFloat3("gridColour", m_colour.x, m_colour.y, m_colour.z);
}

glm::vec3 Grid::GetColour() { return m_colour; }


glm::vec2 Grid::NearestCenter(float shapeSize, glm::vec2 pos)
{
    float size = std::min(m_scale, SnapGridSize(shapeSize));
    float halfSize = size * 0.5f;
    return glm::vec2(
        int(pos.x / size) * size + (pos.x > 0 ? halfSize : -halfSize),
        int(pos.y / size) * size + (pos.y > 0 ? halfSize : -halfSize)
    );
}

glm::vec2 Grid::NearestCorner(float shapeSize, glm::vec2 pos)
{
    float size = std::min(m_scale, SnapGridSize(shapeSize));
    return glm::vec2(int(pos.x / size) * size, int(pos.y / size) * size);
}

ShapeGridSize Grid::GetShapeGridSize(Shape2D* shape)
{
    float size = shape->GetModel()->GetScalef();
    if (size >= m_scale)
        return static_cast<ShapeGridSize>(std::round(size / m_scale) - 1);
    else
        return static_cast<ShapeGridSize>(1 - std::round(m_scale / size));
}

glm::vec2 Grid::ShapeSnapPosition(Shape2D* shape, glm::vec2 pos)
{
    int gridSize = GetShapeGridSize(shape);
    if (gridSize < 0 || gridSize % 2 == 0)
        return NearestCenter(shape->GetModel()->GetScalef(), pos);
    else
        return NearestCorner(shape->GetModel()->GetScalef(), pos);
}

float Grid::SnapGridSize(ShapeGridSize shapeGridSize)
{
    if (shapeGridSize >= 0)
        return m_scale * (shapeGridSize + 1);
    else
        return m_scale / (float)-(shapeGridSize-1);
}

float Grid::SnapGridSize(float size)
{
    if (size >= m_scale)
        return std::round(size / m_scale) * m_scale;
    else
        return m_scale / std::round(m_scale / size);
}

nlohmann::json Grid::Serialize() const
{
    return {
        {"scale", m_scale}
    };
}

void Grid::Deserialize(nlohmann::json json)
{
    m_scale = json["scale"];
}
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


glm::vec2 Grid::NearestCenter(glm::vec2 pos)
{
    float halfScale = m_scale * 0.5f;
    return glm::vec2(
        int(pos.x / m_scale) * m_scale + (pos.x > 0 ? halfScale : -halfScale),
        int(pos.y / m_scale) * m_scale + (pos.y > 0 ? halfScale : -halfScale)
    );
}

glm::vec2 Grid::NearestCorner(glm::vec2 pos)
{
    return glm::vec2(int(pos.x / m_scale) * m_scale, int(pos.y / m_scale) * m_scale);
}

int Grid::TokenGridSize(Token* token)
{
    return std::round(token->GetSize() / GetScale());
}

glm::vec2 Grid::TokenSnapPosition(Token* token, glm::vec2 pos)
{
    if (TokenGridSize(token) % 2 == 0)
        return NearestCorner(pos);
    else
        return NearestCenter(pos);
}

float Grid::SnapGridSize(float size)
{
    if (size >= m_scale)
        return std::round(size / m_scale) * m_scale;
    else
        return m_scale / std::round(m_scale / size);
}

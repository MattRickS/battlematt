#include <glm/glm.hpp>

#include <Primitives.h>
#include <Shader.h>
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


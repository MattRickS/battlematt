#include <iostream>
#include <memory>

#include <glm/glm.hpp>

#include <Mesh.h>
#include <Shader.h>

#include <Overlays.h>


RectOverlay::RectOverlay(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader) : m_mesh(mesh), m_shader(shader)
{
    SetColour(colour);
}

void RectOverlay::Draw()
{
    m_shader->use();
    m_shader->setFloat4("coords", MinX(), MinY(), MaxX(), MaxY());
    m_mesh->Draw(*m_shader);
}

void RectOverlay::SetColour(glm::vec3 col)
{
    colour = col;
    m_shader->use();
    m_shader->setFloat3("colour", colour.x, colour.y, colour.z);
}

float RectOverlay::MinX() { return std::min(startCorner.x, endCorner.x); }
float RectOverlay::MaxX() { return std::max(startCorner.x, endCorner.x); }
float RectOverlay::MinY() { return std::min(startCorner.y, endCorner.y); }
float RectOverlay::MaxY() { return std::max(startCorner.y, endCorner.y); }

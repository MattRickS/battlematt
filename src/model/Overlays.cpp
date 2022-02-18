#include <iostream>
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Mesh.h>
#include <glutil/Shader.h>

#include <model/Overlays.h>


Overlay::Overlay(std::shared_ptr<Shader> shader) : shader(shader) {}

RectOverlay::RectOverlay(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, glm::vec4 colour) : Overlay(shader), Rect(mesh)
{
    SetColour(colour);
}

void RectOverlay::Draw()
{
    shader->use();
    shader->setFloat4("coords", MinX(), MinY(), MaxX(), MaxY());
    m_mesh->Draw(*shader);
}

void RectOverlay::SetColour(glm::vec4 col)
{
    m_colour = col;
    shader->use();
    shader->setFloat4("colour", m_colour.x, m_colour.y, m_colour.z, m_colour.w);
}

float RectOverlay::MinX() { return std::min(startCorner.x, endCorner.x); }
float RectOverlay::MaxX() { return std::max(startCorner.x, endCorner.x); }
float RectOverlay::MinY() { return std::min(startCorner.y, endCorner.y); }
float RectOverlay::MaxY() { return std::max(startCorner.y, endCorner.y); }

void RectOverlay::SetCorners(glm::vec2 pos)
{
    startCorner = endCorner = pos;
    m_model->SetPos(pos);
    m_model->SetScalef(0.0f);
}

void RectOverlay::SetEndCorner(glm::vec2 pos)
{
    
}

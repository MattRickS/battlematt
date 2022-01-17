#include <iostream>
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Mesh.h>
#include <glutil/Shader.h>

#include <model/Overlays.h>


Overlay::Overlay(std::shared_ptr<Shader> shader) : shader(shader) {}

RectOverlay::RectOverlay(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader) : Overlay(shader), m_mesh(mesh)
{
    SetColour(colour);
}

void RectOverlay::Draw()
{
    shader->use();
    shader->setFloat4("coords", MinX(), MinY(), MaxX(), MaxY());
    m_mesh->Draw(*shader);
}

void RectOverlay::SetColour(glm::vec3 col)
{
    colour = col;
    shader->use();
    shader->setFloat3("colour", colour.x, colour.y, colour.z);
}

float RectOverlay::MinX() { return std::min(startCorner.x, endCorner.x); }
float RectOverlay::MaxX() { return std::max(startCorner.x, endCorner.x); }
float RectOverlay::MinY() { return std::min(startCorner.y, endCorner.y); }
float RectOverlay::MaxY() { return std::max(startCorner.y, endCorner.y); }

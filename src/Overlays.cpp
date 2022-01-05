#include <iostream>

#include <glm/glm.hpp>

#include <Primitives.h>
#include <Shader.h>

#include <Overlays.h>


RectOverlay::RectOverlay() : mesh(2), shader("resources/shaders/Grid.vs", "resources/shaders/Rect.fs")
{
    SetColour(colour);
}

void RectOverlay::Draw()
{
    shader.use();
    shader.setFloat4("coords", MinX(), MinY(), MaxX(), MaxY());
    mesh.Draw(shader);
}

void RectOverlay::SetColour(glm::vec3 col)
{
    colour = col;
    shader.use();
    shader.setFloat3("colour", colour.x, colour.y, colour.z);
}

float RectOverlay::MinX() { return std::min(startCorner.x, endCorner.x); }
float RectOverlay::MaxX() { return std::max(startCorner.x, endCorner.x); }
float RectOverlay::MinY() { return std::min(startCorner.y, endCorner.y); }
float RectOverlay::MaxY() { return std::max(startCorner.y, endCorner.y); }

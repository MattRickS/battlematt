#pragma once
#include <memory>
#include <glm/glm.hpp>

#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <model/Shape2D.h>


class Overlay
{
public:
    std::shared_ptr<Shader> shader;

    Overlay(std::shared_ptr<Shader> shader);

    virtual void Draw() = 0;
};


class RectOverlay : public Overlay, public Rect
{
public:
    RectOverlay(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, glm::vec4 colour=glm::vec4(1));
    virtual void Draw();
    void SetColour(glm::vec4 col);
    float MinX();
    float MaxX();
    float MinY();
    float MaxY();
    void SetCorners(glm::vec2 pos);
    void SetEndCorner(glm::vec2 pos);

private:
    glm::vec4 m_colour;
    glm::vec2 startCorner;
    glm::vec2 endCorner;
};

#pragma once
#include <glm/glm.hpp>

#include <Primitives.h>
#include <Shader.h>


class Overlay
{
    virtual void Draw() = 0;
};


class RectOverlay : public Overlay
{
public:
    glm::vec2 startCorner;
    glm::vec2 endCorner;

    RectOverlay();
    virtual void Draw();
    void SetColour(glm::vec3 col);
    float MinX();
    float MaxX();
    float MinY();
    float MaxY();

private:
    Quad mesh;
    Shader shader;
    glm::vec3 colour = glm::vec3(0, 1, 1);
};

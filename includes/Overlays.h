#pragma once
#include <memory>
#include <glm/glm.hpp>

#include <Mesh.h>
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

    RectOverlay(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader);
    virtual void Draw();
    void SetColour(glm::vec3 col);
    float MinX();
    float MaxX();
    float MinY();
    float MaxY();

private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
    glm::vec3 colour = glm::vec3(0, 1, 1);
};

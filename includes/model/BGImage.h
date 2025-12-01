#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <glutil/Texture.h>
#include <model/Shape2D.h>


class BGImage: public Rect
{
public:

    BGImage(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
    void Draw(Shader &shader) override;
    std::shared_ptr<Texture> GetImage();
    void SetImage(std::shared_ptr<Texture> texture);
    void SetTint(glm::vec4 colour);
    bool GetLockRatio();
    void SetLockRatio(bool lockRatio);
    bool IsVisible() { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }

private:
    std::shared_ptr<Texture> m_texture;
    glm::vec4 m_tintColour = glm::vec4(1);
    bool m_lockRatio = false;
    bool m_visible = true;
};

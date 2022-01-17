#pragma once
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <Matrix2D.h>
#include <Mesh.h>
#include <Shader.h>
#include <Shape2D.h>
#include <Texture.h>


const glm::vec4 highlightColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

class Token : public Rect
{
public:
    std::string name;
    glm::vec4 borderColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    float borderWidth = 0.15f;
    bool isHighlighted = false;
    bool isSelected = false;

    Token(std::shared_ptr<Mesh> mesh);
    Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
    Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::string name);
    void SetIcon(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetIcon();
    virtual void Draw(Shader &shader);
    virtual bool Contains(glm::vec2 pt) const;
    virtual bool Contains(float x, float y) const;

private:
    std::shared_ptr<Texture> m_texture;
};

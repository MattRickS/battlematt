#pragma once
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <glutil/Texture.h>
#include <model/Shape2D.h>


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

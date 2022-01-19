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
    bool isHighlighted = false;
    bool isSelected = false;

    Token(std::shared_ptr<Mesh> mesh);
    Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
    Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::string name);
    void SetIcon(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetIcon();
    void SetBorderWidth(float width);
    float GetBorderWidth();
    void SetBorderColor(glm::vec4 color);
    glm::vec4 GetBorderColor();
    void SetName(std::string name);
    std::string GetName();
    virtual void Draw(Shader &shader);
    virtual bool Contains(glm::vec2 pt) const;
    virtual bool Contains(float x, float y) const;

private:
    std::string m_name;
    std::shared_ptr<Texture> m_texture;
    glm::vec4 m_borderColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    float m_borderWidth = 0.15f;
};

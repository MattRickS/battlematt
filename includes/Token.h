#pragma once
#include <string>
#include <glm/glm.hpp>

#include <Primitives.h>
#include <Texture.h>
#include <Shader.h>


class Token : public Quad
{
public:
    std::string iconPath;
    glm::vec4 borderColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    float borderWidth = 0.1f;
    bool isHighlighted = false;
    bool isSelected = false;

    Token(std::string iconPath);
    void SetUVOffset(glm::vec2 offset);
    void SetSize(float size);
    void SetPos(glm::vec3 pos);
    const glm::mat4* GetModel() const;
    void Draw(Shader &shader);
    bool Contains(glm::vec2 pt) const;
    bool Contains(float x, float y) const;

private:
    Texture& tex;
    glm::vec2 iconOffset =glm::vec2(0.0f, 0.0f); // Defaults to centered on the given image
    glm::mat4 model = glm::mat4(1.0f);
    float m_scale = 1.0f;
    glm::vec3 m_pos =  glm::vec3(0.0f, 0.0f, 0.0f);

    void RebuildModel();
};

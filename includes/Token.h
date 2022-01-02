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

    Token(std::string iconPath);
    void SetUVOffset(glm::vec2 offset);
    void SetSize(float size);
    void SetPos(glm::vec3 pos);
    glm::mat4& GetModel();
    void Draw(Shader &shader);

private:
    Texture& tex;
    glm::vec2 iconOffset =glm::vec2(0.0f, 0.0f); // Defaults to centered on the given image
    glm::mat4 model = glm::mat4(1.0f);
    float m_scale = 1.0f;
    glm::vec3 m_pos =  glm::vec3(0.0f, 0.0f, 0.0f);

    void RebuildModel();
};

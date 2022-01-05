#pragma once
#include <string>

#include <glm/glm.hpp>

#include <Texture.h>
#include <Primitives.h>


class BGImage: public Quad
{
public:
    BGImage(std::string texturePath);
    void SetPos(glm::vec3 pos);
    void SetScale(float scale);
    void SetScale(glm::vec3 scale);
    float GetScale() { return m_scale.x; }
    const glm::mat4* GetModel() const { return &m_model; }
    glm::vec2 GetDimensions() { return glm::vec2(tex->width, tex->height); }
    void Draw(Shader &shader);
    std::string GetImage();
    void SetImage(std::string imagePath);

private:
    Texture* tex;
    glm::mat4 m_model;
    glm::vec3 m_pos = glm::vec3(0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f);

    void RebuildModel();
};

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
    float GetScale() { return m_scale; }
    const glm::mat4* GetModel() const { return &m_model; }
    void Draw(Shader &shader);
    std::string GetImage();
    void SetImage(std::string imagePath);

private:
    Texture* tex;
    glm::mat4 m_model;
    glm::vec3 m_pos = glm::vec3(0.0f);
    float m_scale = 1.0f;

    void RebuildModel();
};

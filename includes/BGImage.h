#pragma once
#include <string>

#include <glm/glm.hpp>
#include <json.hpp>

#include <Texture.h>
#include <Primitives.h>


class BGImage: public Quad
{
public:
    BGImage(std::string texturePath);
    void SetPos(glm::vec3 pos);
    glm::vec3 GetPos();
    void SetScale(glm::vec2 scale);
    glm::vec2 GetScale() { return m_scale; }
    void SetRotation(float degrees);
    float GetRotation();
    const glm::mat4* GetModel() const { return &m_model; }
    void Draw(Shader &shader);
    std::string GetImage();
    void SetImage(std::string imagePath);
    nlohmann::json Serialize() const;
    void Deserialize(nlohmann::json json);

private:
    Texture* tex;
    glm::mat4 m_model;
    glm::vec3 m_pos = glm::vec3(0.0f);
    glm::vec2 m_scale = glm::vec2(1.0f);
    float m_rot = 0.0f;

    void RebuildModel();
};

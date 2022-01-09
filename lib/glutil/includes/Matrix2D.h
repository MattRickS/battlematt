#pragma once
#include <glm/glm.hpp>
#include <json.hpp>

class Matrix2D
{
public:
    Matrix2D();

    void Offset(glm::vec2 offset);

    void SetPos(glm::vec2 pos);
    glm::vec2 GetPos() const;

    void SetScale(glm::vec2 scale);
    glm::vec2 GetScale() const;
    void SetScalef(float scale);
    float GetScalef() const;

    void SetRotation(float degrees);
    float GetRotation() const;

    const glm::mat4* Value() const;

    nlohmann::json Serialize() const;
    void Deserialize(nlohmann::json json);

    void Rebuild();

private:
    glm::mat4 m_matrix;
    glm::vec2 m_pos;
    glm::vec2 m_scale;
    float m_rot;
};

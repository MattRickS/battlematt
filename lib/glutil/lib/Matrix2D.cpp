#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json.hpp>

#include <Matrix2D.h>


Matrix2D::Matrix2D() : m_pos(0), m_scale(1), m_rot(0) { Rebuild(); }

void Matrix2D::Offset(glm::vec2 offset)
{
    m_pos += offset;
    Rebuild();
}

void Matrix2D::SetPos(glm::vec2 pos)
{
    m_pos = pos;
    Rebuild();
}

void Matrix2D::SetRotation(float degrees)
{
    m_rot = degrees;
    Rebuild();
}

void Matrix2D::SetScale(glm::vec2 scale)
{
    m_scale = scale;
    Rebuild();
}
void Matrix2D::SetScalef(float scale)
{
    m_scale = glm::vec2(scale);
    Rebuild();
}

glm::vec2 Matrix2D::GetPos() const { return m_pos; }
glm::vec2 Matrix2D::GetScale() const { return m_scale; }
float Matrix2D::GetScalef() const { return m_scale.x; }
float Matrix2D::GetRotation() const { return m_rot; }
const glm::mat4* Matrix2D::Value() const { return &m_matrix; }

nlohmann::json Matrix2D::Serialize() const
{
    return {
        {"pos", {m_pos.x, m_pos.y}},
        {"scale", {m_scale.x, m_scale.y}},
        {"rotation", m_rot},
    };
}
void Matrix2D::Deserialize(nlohmann::json json)
{
    m_pos = glm::vec2(json["pos"][0], json["pos"][1]);
    m_scale = glm::vec2(json["scale"][0], json["scale"][1]);
    m_rot = json["rotation"];
    Rebuild();
}

void Matrix2D::Rebuild()
{
    m_matrix = glm::mat4(1.0f);
    m_matrix = glm::translate(m_matrix, glm::vec3(m_pos, 0.0f));
    m_matrix = glm::rotate(m_matrix, glm::radians(-m_rot), glm::vec3(0, 0, 1));
    m_matrix = glm::scale(m_matrix, glm::vec3(m_scale, 1.0f));
}

#pragma once
#include <string>

#include <glm/glm.hpp>
#include <json.hpp>

#include <Primitives.h>
#include <Texture.h>
#include <Shader.h>


const glm::vec4 highlightColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

class Token : public Quad
{
public:
    std::string name;
    glm::vec4 borderColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    float borderWidth = 0.15f;
    bool isHighlighted = false;
    bool isSelected = false;

    Token(std::string iconPath);
    Token(std::string iconPath, std::string name);
    void SetIcon(std::string path);
    void SetSize(float size);
    void SetPos(glm::vec3 pos);
    glm::vec3 GetPos() { return m_pos; }
    float GetSize() { return m_scale; }
    std::string GetIcon() { return tex->filename; }
    void Move(glm::vec2 offset);
    void Move(float xoffset, float yoffset);
    const glm::mat4* GetModel() const;
    void Draw(Shader &shader);
    bool Contains(glm::vec2 pt) const;
    bool Contains(float x, float y) const;
    nlohmann::json Serialize() const;

private:
    Texture* tex;
    glm::mat4 model = glm::mat4(1.0f);
    float m_scale = 1.0f;
    glm::vec3 m_pos =  glm::vec3(0.0f, 0.0f, 0.0f);

    void RebuildModel();
};

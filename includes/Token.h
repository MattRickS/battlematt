#pragma once
#include <string>

#include <glm/glm.hpp>
#include <json.hpp>

#include <Matrix2D.h>
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

    Token();
    Token(std::string iconPath);
    Token(std::string iconPath, std::string name);
    void SetIcon(std::string path);
    std::string GetIcon() { return tex->filename; }
    void SetModel(Matrix2D matrix);
    Matrix2D* GetModel();
    void Draw(Shader &shader);
    bool Contains(glm::vec2 pt) const;
    bool Contains(float x, float y) const;
    nlohmann::json Serialize() const;
    void Deserialize(nlohmann::json json);

private:
    Texture* tex;
    Matrix2D m_model;
};

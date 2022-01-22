#pragma once
#include <bitset>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <glutil/Texture.h>
#include <model/Shape2D.h>


const glm::vec4 highlightColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
const unsigned int NUM_TOKEN_STATUSES = 6;
typedef std::bitset<NUM_TOKEN_STATUSES> TokenStatuses;

class Token : public Rect
{
public:
    bool isHighlighted = false;
    bool isSelected = false;

    Token(std::shared_ptr<Mesh> mesh);
    Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
    Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::string name);
    Token(const Token& token);

    void SetIcon(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetIcon();
    void SetBorderWidth(float width);
    float GetBorderWidth();
    void SetBorderColor(glm::vec4 color);
    glm::vec4 GetBorderColor();
    void SetName(std::string name);
    std::string GetName();
    void SetStatuses(TokenStatuses statuses);
    TokenStatuses GetStatuses();
    void SetStatusEnabled(int status, bool enabled);
    bool IsStatusEnabled(int status);
    virtual void Draw(Shader &shader);
    virtual bool Contains(glm::vec2 pt) const;
    virtual bool Contains(float x, float y) const;

private:
    std::string m_name;
    std::shared_ptr<Texture> m_texture;
    glm::vec4 m_borderColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    float m_borderWidth = 0.15f;
    TokenStatuses m_statuses;
};

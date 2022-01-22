#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <model/Token.h>


Token::Token(std::shared_ptr<Mesh> mesh) : Rect(mesh), m_name("") {}
Token::Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture) : Token(mesh, texture, texture->Name()) {}
Token::Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::string name) : Rect(mesh), m_name(name), m_texture(texture) {}
// TODO: Rule of five
Token::Token(const Token& token) : Rect(token)
{
    m_name = token.m_name;
    m_texture = token.m_texture;
    m_model = std::make_shared<Matrix2D>(*token.m_model);
    m_borderColor = token.m_borderColor;
    m_borderWidth = token.m_borderWidth;
}

void Token::SetIcon(std::shared_ptr<Texture> texture) { m_texture = texture; }
std::shared_ptr<Texture> Token::GetIcon() { return m_texture; }
void Token::SetBorderWidth(float width) { m_borderWidth = width; }
float Token::GetBorderWidth() { return m_borderWidth; }
void Token::SetBorderColor(glm::vec4 color) { m_borderColor = color; }
glm::vec4 Token::GetBorderColor() { return m_borderColor; }
void Token::SetName(std::string name) { m_name = name; }
std::string Token::GetName() { return m_name; }
void Token::SetStatuses(TokenStatuses statuses) { m_statuses = statuses; }
TokenStatuses Token::GetStatuses() { return m_statuses; }
void Token::SetStatusEnabled(int status, bool enabled) { m_statuses[status] = enabled; }
bool Token::IsStatusEnabled(int status) { return m_statuses[status]; }
void Token::SetXStatus(bool enabled) { m_xStatus = enabled; }
bool Token::GetXStatus() { return m_xStatus; }
void Token::SetOpacity(float opacity) { m_opacity = opacity; }
float Token::GetOpacity() { return m_opacity; }

void Token::Draw(Shader &shader)
{
    shader.setMat4("model", *m_model->Value());

    glm::vec4 highlight;
    if (isSelected)
        highlight = highlightColor;
    else if (isHighlighted)
        highlight = highlightColor * 0.75f;
    else
        highlight = glm::vec4(0);

    if (m_texture && m_texture->IsValid())
    {
        m_texture->activate(GL_TEXTURE0);
        shader.setInt("diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, m_texture->ID);
    }

    shader.setFloat4("highlightColor", highlight.x, highlight.y, highlight.z, highlight.w);
    shader.setFloat4("borderColor", m_borderColor.x, m_borderColor.y, m_borderColor.z, m_borderColor.w);
    shader.setFloat("borderWidth", m_borderWidth);
    shader.setFloat("opacity", m_opacity);
    Rect::Draw(shader);
}

bool Token::Contains(float x, float y) const
{
    return Contains(glm::vec2(x, y));
}


bool Token::Contains(glm::vec2 pt) const
{
    // Scale is the diameter, use radius for comparison
    return glm::length(m_model->GetPos() - pt) < (m_model->GetScalef() * 0.5f);
}

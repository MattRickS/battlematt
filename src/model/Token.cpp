#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <model/Token.h>


Token::Token(std::shared_ptr<Mesh> mesh) : Rect(mesh), name("") {}
Token::Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture) : Token(mesh, texture, texture->Name()) {}
Token::Token(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::string name) : Rect(mesh), name(name), m_texture(texture) {}

void Token::SetIcon(std::shared_ptr<Texture> texture) { m_texture = texture; }
std::shared_ptr<Texture> Token::GetIcon() { return m_texture; }

void Token::Draw(Shader &shader)
{
    shader.setMat4("model", *m_model.Value());

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
    shader.setFloat4("borderColor", borderColor.x, borderColor.y, borderColor.z, borderColor.w);
    shader.setFloat("borderWidth", borderWidth);
    Rect::Draw(shader);
}

bool Token::Contains(float x, float y) const
{
    return Contains(glm::vec2(x, y));
}


bool Token::Contains(glm::vec2 pt) const
{
    // Scale is the diameter, use radius for comparison
    return glm::length(m_model.GetPos() - pt) < (m_model.GetScalef() * 0.5f);
}

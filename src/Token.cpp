#include <filesystem>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json.hpp>

#include <Matrix2D.h>
#include <Token.h>
#include <Primitives.h>
#include <TextureCache.h>

Token::Token() : Quad(), name("") {}

Token::Token(std::string iconPath) : Token(iconPath, std::filesystem::path(iconPath).stem()) {}

Token::Token(std::string iconPath, std::string name) : Quad(), name(name), tex(TextureCache::GetTexture(iconPath))
{}

void Token::SetIcon(std::string path)
{
    tex = TextureCache::GetTexture(path);
}

Matrix2D* Token::GetModel() { return &m_model; }

void Token::SetModel(Matrix2D matrix) { m_model = matrix; }

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

    if (tex && tex->IsValid())
    {
        tex->activate(GL_TEXTURE0);
        shader.setInt("diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, tex->ID);
    }

    shader.setFloat4("highlightColor", highlight.x, highlight.y, highlight.z, highlight.w);
    shader.setFloat4("borderColor", borderColor.x, borderColor.y, borderColor.z, borderColor.w);
    shader.setFloat("borderWidth", borderWidth);
    Quad::Draw(shader);
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


nlohmann::json Token::Serialize() const
{
    return {
        {"name", name},
        {"texture", tex->filename},
        {"matrix2D", m_model.Serialize()},
        {"borderWidth", borderWidth},
        {"borderColour", {borderColor.x, borderColor.y, borderColor.z, borderColor.w}}
    };
}

void Token::Deserialize(nlohmann::json json)
{
    m_model.Deserialize(json["matrix2D"]);
    SetIcon(json["texture"]);
    name = json["name"];
    borderWidth = json["borderWidth"];
    borderColor = glm::vec4(
        json["borderColour"][0], json["borderColour"][1], json["borderColour"][2], json["borderColour"][3]
    );
}
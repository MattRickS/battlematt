#include <filesystem>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json.hpp>

#include <Token.h>
#include <Primitives.h>
#include <TextureCache.h>

Token::Token() : Quad(), name("") {}

Token::Token(std::string iconPath) : Token(iconPath, std::filesystem::path(iconPath).stem()) {}

Token::Token(std::string iconPath, std::string name) : Quad(), name(name), tex(TextureCache::GetTexture(iconPath))
{
    RebuildModel();
}

void Token::SetIcon(std::string path)
{
    tex = TextureCache::GetTexture(path);
}

void Token::SetSize(float size)
{
    m_scale = size;
    RebuildModel();
}

void Token::SetPos(glm::vec3 pos)
{
    m_pos = pos;
    RebuildModel();
}

void Token::Move(glm::vec2 offset)
{
    Move(offset.x, offset.y);
}

void Token::Move(float xoffset, float yoffset)
{
    m_pos += glm::vec3(xoffset, yoffset, 0);
    RebuildModel();
}

const glm::mat4* Token::GetModel() const { return &model; }

void Token::Draw(Shader &shader)
{
    shader.setMat4("model", *GetModel());

    glm::vec4 highlight;
    if (isSelected)
        highlight = highlightColor;
    else if (isHighlighted)
        highlight = highlightColor * 0.75f;
    else
        highlight = glm::vec4(0);

    if (tex)
    {
        tex->activate(GL_TEXTURE0);
        shader.setInt("diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, tex->ID);
    }
    else
        std::cerr << "No texture assigned to Token" << std::endl;

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
    return glm::length(glm::vec2(m_pos) - pt) < (m_scale * 0.5f);
}


void Token::RebuildModel()
{
    // TODO: Model should be separated from the rest of the token properties so that it can be drawn as multiple instances
    model = glm::mat4(1.0f);
    model = glm::translate(model, m_pos);
    model = glm::scale(model, glm::vec3(m_scale));
    // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
}

nlohmann::json Token::Serialize() const
{
    return {
        {"name", name},
        {"texture", tex->filename},
        {"pos", {m_pos.x, m_pos.y, m_pos.z}},
        {"scale", m_scale},
        {"borderWidth", borderWidth},
        {"borderColour", {borderColor.x, borderColor.y, borderColor.z, borderColor.w}}
    };
}

void Token::Deserialize(nlohmann::json json)
{
    SetIcon(json["texture"]);
    SetSize(json["scale"]);
    SetPos(glm::vec3(json["pos"][0], json["pos"][1], json["pos"][2]));
    name = json["name"];
    borderWidth = json["borderWidth"];
    borderColor = glm::vec4(
        json["borderColour"][0], json["borderColour"][1], json["borderColour"][2], json["borderColour"][3]
    );
}
#include <filesystem>
#include <string>

#include <glm/glm.hpp>

#include <Token.h>
#include <Primitives.h>
#include <TextureCache.h>


Token::Token(std::string iconPath) : Token(iconPath, std::filesystem::path(iconPath).stem()) {}

Token::Token(std::string iconPath, std::string name) : Quad(), iconPath(iconPath), name(name), tex(TextureCache::GetTexture(iconPath))
{
    // TODO: This isn't working
    if (tex.width != tex.height)
    {
        float xoff = std::max(1.0f, tex.height / (float)tex.width);
        float yoff = std::max(1.0f, tex.width / (float)tex.height);
        for (Vertex& vert : vertices)
            vert.TexCoords = glm::vec2((vert.TexCoords.x - 0.5f) * xoff, (vert.TexCoords.y - 0.5f) * yoff);
    }
    RebuildModel();
}

void Token::SetUVOffset(glm::vec2 offset)
{
    // Undo the previous offset before adding the new one
    for (Vertex& vert : vertices)
        vert.TexCoords -= iconOffset + offset;

    iconOffset = offset;
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

    tex.activate(GL_TEXTURE0);
    shader.setInt("diffuse", 0);
    shader.setFloat4("highlightColor", highlight.x, highlight.y, highlight.z, highlight.w);
    shader.setFloat4("borderColor", borderColor.x, borderColor.y, borderColor.z, borderColor.w);
    shader.setFloat("borderWidth", borderWidth);
    glBindTexture(GL_TEXTURE_2D, tex.ID);
    Quad::Draw(shader);
}

bool Token::Contains(float x, float y) const
{
    return Contains(glm::vec2(x, y));
}


bool Token::Contains(glm::vec2 pt) const
{
    return glm::length(glm::vec2(m_pos) - pt) < m_scale;
}


void Token::RebuildModel()
{
    // TODO: Model should be separated from the rest of the token properties so that it can be drawn as multiple instances
    model = glm::mat4(1.0f);
    model = glm::translate(model, m_pos);
    model = glm::scale(model, glm::vec3(m_scale));
    // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
}

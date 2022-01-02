#include <string>
#include <glm/glm.hpp>

#include <Token.h>
#include <Primitives.h>
#include <TextureCache.h>


Token::Token(std::string iconPath) : Quad(), iconPath(iconPath), tex(TextureCache::GetTexture(iconPath))
{
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

const glm::mat4* Token::GetModel() const { return &model; }

void Token::Draw(Shader &shader)
{
    shader.setMat4("model", *GetModel());

    glm::vec4 border;
    if (isSelected)
        border = 1.0f - borderColor;
    else if (isHighlighted)
        border = borderColor * 0.8f;
    else
        border = borderColor;

    tex.activate(GL_TEXTURE0);
    shader.setInt("diffuse", 0);
    shader.setFloat4("borderColor", border.x, border.y, border.z, border.w);
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

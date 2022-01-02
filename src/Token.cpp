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
    model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
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

glm::mat4& Token::GetModel() { return model; }

// override Draw to activate image/set border settings for shader
void Token::Draw(Shader &shader)
{
    shader.setMat4("model", GetModel());

    tex.activate(GL_TEXTURE0);
    shader.setInt("diffuse", 0);
    shader.setFloat4("borderColor", borderColor.x, borderColor.y, borderColor.z, borderColor.w);
    glBindTexture(GL_TEXTURE_2D, tex.ID);
    Mesh::Draw(shader);
}


void Token::RebuildModel()
{
    // TODO: Pretty sure this is scaling the translation, but changing order makes it disappear entirely
    // TODO: Model should be separated from the rest of the token properties so that it can be drawn as multiple instances
    model = glm::scale(model, glm::vec3(m_scale));
    model = glm::translate(model, m_pos);
    model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
}

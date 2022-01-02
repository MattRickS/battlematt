#include <string>
#include <glm/glm.hpp>

#include <Primitives.hpp>
#include <TextureCache.hpp>


class Token : public Quad
{
public:
    std::string iconPath;
    glm::vec4 borderColor {1.0f, 0.0f, 0.0f, 1.0f};
    float borderWidth {0.1f};

    Token(std::string iconPath) : Quad(), iconPath(iconPath), tex(TextureCache::GetTexture(iconPath))
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

    void SetUVOffset(glm::vec2 offset)
    {
        // Undo the previous offset before adding the new one
        for (Vertex& vert : vertices)
            vert.TexCoords -= iconOffset + offset;

        iconOffset = offset;
    }

    void SetSize(float size)
    {
        m_scale = size;
        RebuildModel();
    }

    void SetPos(glm::vec3 pos)
    {
        m_pos = pos;
        RebuildModel();
    }

    glm::mat4& GetModel()
    {
        return model;
    }

    // override Draw to activate image/set border settings for shader
    void Draw(Shader &shader)
    {
        shader.setMat4("model", GetModel());

        tex.activate(GL_TEXTURE0);
        shader.setInt("diffuse", 0);
        shader.setFloat4("borderColor", borderColor.x, borderColor.y, borderColor.z, borderColor.w);
        glBindTexture(GL_TEXTURE_2D, tex.ID);
        Mesh::Draw(shader);
    }

private:
    Texture& tex;
    glm::vec2 iconOffset {0.0f, 0.0f}; // Defaults to centered on the given image
    glm::mat4 model {1.0f};
    float m_scale {1.0f};
    glm::vec3 m_pos {0.0f, 0.0f, 0.0f};

    void RebuildModel()
    {
        // TODO: Pretty sure this is scaling the translation, but changing order makes it disappear entirely
        // TODO: Model should be separated from the rest of the token properties so that it can be drawn as multiple instances
        model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(m_scale));
        model = glm::translate(model, m_pos);
    }
};

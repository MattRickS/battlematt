#include <string>

#include <glm/glm.hpp>

#include <TextureCache.h>
#include <Primitives.h>
#include <BGImage.h>


BGImage::BGImage(std::string texturePath) : Quad(), tex(TextureCache::GetTexture(texturePath))
{
    RebuildModel();
}

void BGImage::SetPos(glm::vec3 pos)
{
    this->m_pos = pos;
    RebuildModel();
}

void BGImage::SetScale(glm::vec3 scale)
{
    this->m_scale = scale;
    RebuildModel();
}

void BGImage::SetScale(float scale)
{
    this->m_scale = glm::vec3(scale);
    RebuildModel();
}

void BGImage::Draw(Shader &shader)
{
    shader.setMat4("model", *GetModel());

    tex.activate(GL_TEXTURE0);
    shader.setInt("diffuse", 0);
    glBindTexture(GL_TEXTURE_2D, tex.ID);
    Quad::Draw(shader);
}

void BGImage::RebuildModel()
{
    m_model = glm::mat4(1.0f);
    m_model = glm::translate(m_model, m_pos);
    m_model = glm::scale(m_model, glm::vec3(m_scale));
}

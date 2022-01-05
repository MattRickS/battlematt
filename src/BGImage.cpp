#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <TextureCache.h>
#include <Primitives.h>
#include <BGImage.h>


const float DEFAULT_PIXELS_PER_UNIT = 50.0f;


BGImage::BGImage(std::string texturePath) : Quad(), tex(TextureCache::GetTexture(texturePath))
{
    m_scale = tex->width / DEFAULT_PIXELS_PER_UNIT;
    RebuildModel();
}

void BGImage::SetPos(glm::vec3 pos)
{
    m_pos = pos;
    RebuildModel();
}

void BGImage::SetScale(float scale)
{
    m_scale = scale;
    RebuildModel();
}

void BGImage::Draw(Shader &shader)
{
    shader.setMat4("model", *GetModel());

    if (tex)
    {
        tex->activate(GL_TEXTURE0);
        shader.setInt("diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, tex->ID);
    }
    else
        std::cerr << "No texture assigned to BGImage" << std::endl;
    Quad::Draw(shader);
}

void BGImage::RebuildModel()
{
    m_model = glm::mat4(1.0f);
    m_model = glm::translate(m_model, m_pos);
    m_model = glm::scale(m_model, glm::vec3(m_scale));
}


std::string BGImage::GetImage()
{
    return tex->filename;
}

void BGImage::SetImage(std::string imagePath)
{
    tex = TextureCache::GetTexture(imagePath);
    if (!m_scale)
        m_scale = tex->width / DEFAULT_PIXELS_PER_UNIT;
    RebuildModel();
}

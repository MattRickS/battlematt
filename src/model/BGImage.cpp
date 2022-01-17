#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <glutil/Texture.h>
#include <model/Shape2D.h>

#include <model/BGImage.h>


const float DEFAULT_PIXELS_PER_UNIT = 50.0f;


BGImage::BGImage(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture) : Rect(mesh), m_texture(texture)
{
    if (m_texture->IsValid())
        m_model.SetScale(glm::vec2(m_texture->width / DEFAULT_PIXELS_PER_UNIT, m_texture->width / DEFAULT_PIXELS_PER_UNIT));
}

void BGImage::Draw(Shader &shader)
{
    shader.setMat4("model", *m_model.Value());

    if (m_texture && m_texture->IsValid())
    {
        m_texture->activate(GL_TEXTURE0);
        shader.setInt("diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, m_texture->ID);
    }
    Rect::Draw(shader);
}

std::shared_ptr<Texture> BGImage::GetImage()
{
    return m_texture;
}

void BGImage::SetImage(std::shared_ptr<Texture> texture)
{
    if (!lockRatio)
    {
        // Resize the incoming image to match the width of the current image
        float hSize = m_texture->width * m_model.GetScale().x;
        m_texture = texture;
        m_model.SetScale(glm::vec2(hSize / texture->width, hSize / texture->height));
    }
    else
    {
        m_texture = texture;
        m_model.Rebuild();
    }
}

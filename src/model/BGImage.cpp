#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Constants.h>
#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <glutil/Texture.h>
#include <model/Shape2D.h>

#include <model/BGImage.h>


BGImage::BGImage(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture) : Rect(mesh), m_texture(texture)
{
    if (m_texture->IsValid())
        m_model->SetScale(glm::vec2(m_texture->width / DEFAULT_PIXELS_PER_UNIT, m_texture->width / DEFAULT_PIXELS_PER_UNIT));
}

void BGImage::Draw(Shader &shader)
{
    glm::vec4 colour = m_color;
    if (isSelected)
        colour = glm::vec4(SELECTION_COLOR, m_color.w);
    else if (isHighlighted)
        colour = glm::vec4(HIGHLIGHT_COLOR, m_color.w);
    shader.setFloat4("color", colour.x, colour.y, colour.z, colour.w);

    if (m_texture && m_texture->IsValid())
    {
        m_texture->activate(GL_TEXTURE0);
        shader.setInt("diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, m_texture->ID);
    }
    // Calling base method will replace colour
    shader.setMat4("model", *m_model->Value());
    m_mesh->Draw(shader);
}

std::shared_ptr<Texture> BGImage::GetImage()
{
    return m_texture;
}

void BGImage::SetImage(std::shared_ptr<Texture> texture)
{
    if (!m_lockRatio)
    {
        // Resize the incoming image to match the width of the current image
        float hSize = m_texture->width * m_model->GetScale().x;
        m_texture = texture;
        m_model->SetScale(glm::vec2(hSize / texture->width, hSize / texture->height));
    }
    else
    {
        m_texture = texture;
        m_model->Rebuild();
    }
}

bool BGImage::GetLockRatio() { return m_lockRatio; }
void BGImage::SetLockRatio(bool lockRatio) { m_lockRatio = lockRatio; }


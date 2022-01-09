#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json.hpp>

#include <Matrix2D.h>
#include <TextureCache.h>
#include <Primitives.h>
#include <BGImage.h>
#include <Shape2D.h>


const float DEFAULT_PIXELS_PER_UNIT = 50.0f;


BGImage::BGImage(std::string texturePath) : Rect(), tex(TextureCache::GetTexture(texturePath))
{
    if (tex->IsValid())
        m_model.SetScale(glm::vec2(tex->width / DEFAULT_PIXELS_PER_UNIT, tex->width / DEFAULT_PIXELS_PER_UNIT));
}

void BGImage::Draw(Shader &shader)
{
    shader.setMat4("model", *m_model.Value());

    if (tex && tex->IsValid())
    {
        tex->activate(GL_TEXTURE0);
        shader.setInt("diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, tex->ID);
    }
    Rect::Draw(shader);
}

std::string BGImage::GetImage()
{
    return tex->filename;
}

void BGImage::SetImage(std::string imagePath)
{
    if (tex->IsValid())
    {
        // Resize the incoming image to match the width of the current image
        float hSize = tex->width * m_model.GetScale().x;
        tex = TextureCache::GetTexture(imagePath);
        m_model.SetScale(glm::vec2(hSize / tex->width, hSize / tex->height));
    }
    else
    {
        tex = TextureCache::GetTexture(imagePath);
        m_model.Rebuild();
    }
}

nlohmann::json BGImage::Serialize() const
{
    return {
        {"texture", tex->filename},
        {"matrix2D", m_model.Serialize()},
    };
}

void BGImage::Deserialize(nlohmann::json json)
{
    SetImage(json["texture"]);
    m_model.Deserialize(json["matrix2D"]);
}

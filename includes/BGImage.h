#include <string>

#include <glm/glm.hpp>

#include <Texture.h>
#include <Primitives.h>


class BGImage: public Quad
{
public:
    Texture& tex;

    BGImage(std::string texturePath);
    void SetPos(glm::vec3 pos);
    void SetScale(float scale);
    void SetScale(glm::vec3 scale);
    glm::mat4& GetModel() { return m_model; }
    void Draw(Shader &shader);

private:
    glm::mat4 m_model;
    glm::vec3 m_pos = glm::vec3(0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f);

    void RebuildModel();
};

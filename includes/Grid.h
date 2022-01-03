#include <glm/glm.hpp>

#include <Primitives.h>
#include <Shader.h>


class Grid
{
public:
    Grid();
    void Draw();
    void SetScale(float scale);
    float GetScale();
    void SetColour(glm::vec3 colour);
    glm::vec3 GetColour();

private:
    Shader shader;
    Quad mesh;
    float m_scale = 1.0f;
    glm::vec3 m_colour = glm::vec3(0.2);
};

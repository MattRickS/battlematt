#pragma once
#include <glm/glm.hpp>

#include <Primitives.h>
#include <Shader.h>
#include <Token.h>


class Grid
{
public:
    Grid();
    void Draw();
    void SetScale(float scale);
    float GetScale();
    void SetColour(glm::vec3 colour);
    glm::vec3 GetColour();
    glm::vec2 NearestCenter(glm::vec2 pos);
    glm::vec2 NearestCorner(glm::vec2 pos);
    int TokenGridSize(Token* token);
    float SnapGridSize(float size);
    glm::vec2 TokenSnapPosition(Token* token, glm::vec2 pos);

private:
    Shader shader;
    Quad mesh;
    float m_scale = 1.0f;
    glm::vec3 m_colour = glm::vec3(0.2);
};

#pragma once
#include <glm/glm.hpp>
#include <json.hpp>

#include <Primitives.h>
#include <Shader.h>
#include <Token.h>


enum TokenGridSize
{
    Fine = -4,
    Diminutive = -3,
    Tiny = -2,
    Small = -1,
    Medium = 0,
    Large = 1,
    Huge = 2,
    Gargantuan = 3,
    Colossal = 4
};


class Grid
{
public:
    Grid();
    void Draw();
    void SetScale(float scale);
    float GetScale();
    void SetColour(glm::vec3 colour);
    glm::vec3 GetColour();
    glm::vec2 NearestCenter(float tokenSize, glm::vec2 pos);
    glm::vec2 NearestCorner(float tokenSize, glm::vec2 pos);
    TokenGridSize GetTokenGridSize(Token* token);
    float SnapGridSize(TokenGridSize tokenGridSize);
    float SnapGridSize(float size);
    glm::vec2 TokenSnapPosition(Token* token, glm::vec2 pos);
    nlohmann::json Serialize() const;
    void Deserialize(nlohmann::json json);

private:
    Shader shader;
    Quad mesh;
    float m_scale = 1.0f;
    glm::vec3 m_colour = glm::vec3(0.2);
};

#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
#include <Shader.h>
#include <Token.h>

class Scene
{
public:
    glm::vec4 bgColor = glm::vec4(0, 0, 0, 1);
    BGImage background;
    Grid grid;
    Camera* camera;
    std::vector<Token> tokens;
    Shader imageShader;
    Shader tokenShader;

    Scene(Camera* camera, std::string bgPath);
    void AddToken(std::string iconPath, glm::vec3 pos=glm::vec3(0));
    void AddToken(std::string iconPath, glm::vec3 pos, float size);
    void Draw();
};
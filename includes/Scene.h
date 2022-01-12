#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
#include <Resources.h>
#include <Shader.h>
#include <Token.h>

class Scene
{
public:
    glm::vec4 bgColor = glm::vec4(0, 0, 0, 1);
    std::vector<std::shared_ptr<BGImage>> backgrounds;
    std::vector<std::shared_ptr<Token>> tokens;
    std::shared_ptr<Grid> grid;
    std::shared_ptr<Camera> camera;

    Scene(std::shared_ptr<Resources> resources, std::shared_ptr<Camera> camera);
    void AddImage(std::string path);
    void AddToken();
    void AddToken(std::string path);
    void AddToken(std::shared_ptr<Token> token);
    void RemoveTokens(std::vector<std::shared_ptr<Token>> toRemove);
    void Draw();

private:
    std::shared_ptr<Resources> m_resources;
};
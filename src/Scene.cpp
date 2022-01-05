#include <algorithm>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
#include <Shader.h>
#include <Token.h>
#include <Scene.h>




Scene::Scene(Camera* camera, std::string bgPath):
    background(bgPath),
    grid(),
    camera(camera),
    imageShader("resources/shaders/SimpleTexture.vs", "resources/shaders/SimpleTexture.fs"),
    tokenShader("resources/shaders/SimpleTexture.vs", "resources/shaders/Token.fs")
{ }

void Scene::AddToken(std::string iconPath, glm::vec3 pos)
{
    tokens.emplace_back(iconPath);
    tokens.back().SetPos(pos);
    tokens.back().SetSize(grid.GetScale());
}

void Scene::AddToken()
{
    tokens.emplace_back("resources/images/QuestionMark.jpg");
}

void Scene::AddToken(std::string iconPath, glm::vec3 pos, float size)
{
    tokens.emplace_back(iconPath);
    tokens.back().SetPos(pos);
    tokens.back().SetSize(size);
}

void Scene::RemoveTokens(std::vector<Token*> toRemove)
{

    auto pred = [&toRemove](const Token& t) ->bool
    {
        return std::find(toRemove.begin(), toRemove.end(), &t) != toRemove.end();
    };

    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), pred), tokens.end());
}


void Scene::Draw()
{
    glClearColor(bgColor.x * bgColor.w, bgColor.y * bgColor.w, bgColor.z * bgColor.w, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    imageShader.use();
    background.Draw(imageShader);
    grid.Draw();

    tokenShader.use();
    for (Token& token : tokens)
        token.Draw(tokenShader);
}
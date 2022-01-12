#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <json.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
#include <Matrix2D.h>
#include <Shader.h>
#include <Resources.h>
#include <Token.h>
#include <Scene.h>


Scene::Scene(std::shared_ptr<Resources> resources, Camera* camera, std::string bgPath):
    background(bgPath),
    grid(),
    camera(camera),
    m_resources(resources)
{ }

void Scene::AddToken(std::string iconPath, glm::vec3 pos)
{
    tokens.emplace_back(iconPath);
    tokens.back().GetModel()->SetPos(pos);
    tokens.back().GetModel()->SetScalef(grid.GetScale());
}

void Scene::AddToken()
{
    tokens.emplace_back("resources/images/QuestionMark.jpg");
}

void Scene::AddToken(std::string iconPath, Matrix2D matrix2D)
{
    tokens.emplace_back(iconPath);
    tokens.back().SetModel(matrix2D);
}

void Scene::RemoveTokens(std::vector<Token*> toRemove)
{

    auto pred = [&toRemove](const Token& t) ->bool
    {
        return std::find(toRemove.begin(), toRemove.end(), &t) != toRemove.end();
    };

    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), pred), tokens.end());
}


// TODO: Move to renderer class
void Scene::Draw()
{
    glClearColor(bgColor.x * bgColor.w, bgColor.y * bgColor.w, bgColor.z * bgColor.w, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    std::shared_ptr<Shader> imageShader = m_resources->GetShader(Resources::ShaderType::Image);
    imageShader->use();
    background.Draw(*imageShader);
    grid.Draw();

    std::shared_ptr<Shader> tokenShader = m_resources->GetShader(Resources::ShaderType::Token);
    tokenShader->use();
    for (Token& token : tokens)
        token.Draw(*tokenShader);
}

nlohmann::json Scene::Serialize() const
{
    std::vector<nlohmann::json> jtokens(tokens.size());
    std::transform(tokens.begin(), tokens.end(), jtokens.begin(), [](const Token& token){ return token.Serialize(); });
    return {
        {"camera", camera->Serialize()},
        {"background", background.Serialize()},
        {"grid", grid.Serialize()},
        {"tokens", jtokens}
    };
}

void Scene::Deserialize(nlohmann::json json)
{
    camera->Deserialize(json["camera"]);
    background.Deserialize(json["background"]);
    grid.Deserialize(json["grid"]);

    tokens.clear();
    uint i = 0;
    nlohmann::json jtokens = json["tokens"];
    tokens = std::vector<Token>(jtokens.size());
    std::for_each(jtokens.begin(), jtokens.end(),
                  [this, &i](const nlohmann::json& jtoken){ tokens[i++].Deserialize(jtoken); });
}

void Scene::Save(std::string path)
{
    std::cerr << "Saving to " << path << std::endl;
    std::ofstream myfile (path);
    if (myfile.is_open())
    {
        myfile << Serialize();
        myfile.close();
    }
    else
        std::cerr << "Unable to open file" << std::endl;
}

void Scene::Load(std::string path)
{
    std::cerr << "Loading Scene from " << path << std::endl;
    nlohmann::json j;
    std::ifstream myfile (path);
    if (myfile.is_open())
    {
        myfile >> j;
        myfile.close();
        Deserialize(j);
    }
    else
        std::cerr << "Unable to open file" << std::endl;
}

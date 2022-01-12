#include <algorithm>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
#include <Shader.h>
#include <Resources.h>
#include <Token.h>
#include <Scene.h>


Scene::Scene(std::shared_ptr<Resources> resources, std::shared_ptr<Camera> camera) : m_resources(resources), camera(camera)
{
    grid = std::make_shared<Grid>(
        m_resources->GetMesh(Resources::MeshType::Quad2),
        m_resources->GetShader(Resources::ShaderType::Grid)
    );
}

void Scene::AddImage(std::string path)
{
    backgrounds.push_back(std::make_shared<BGImage>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(path)
    ));
}

void Scene::AddToken()
{
    tokens.push_back(std::make_shared<Token>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(Resources::TextureType::Default)
    ));
}

void Scene::AddToken(std::string path)
{
    tokens.push_back(std::make_shared<Token>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(path)
    ));
}

void Scene::AddToken(std::shared_ptr<Token> token)
{
    tokens.push_back(token);
}

void Scene::RemoveTokens(std::vector<std::shared_ptr<Token>> toRemove)
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
    for (std::shared_ptr<BGImage>& image: backgrounds)
        image->Draw(*imageShader);

    grid->Draw();

    std::shared_ptr<Shader> tokenShader = m_resources->GetShader(Resources::ShaderType::Token);
    tokenShader->use();
    for (std::shared_ptr<Token> token : tokens)
        token->Draw(*tokenShader);
}

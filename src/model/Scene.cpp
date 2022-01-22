#include <algorithm>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <Resources.h>
#include <glutil/Camera.h>
#include <glutil/Shader.h>
#include <model/BGImage.h>
#include <model/Grid.h>
#include <model/Overlays.h>
#include <model/Token.h>
#include <model/Scene.h>


Scene::Scene(std::shared_ptr<Resources> resources) : m_resources(resources)
{
    grid = std::make_shared<Grid>(
        m_resources->GetMesh(Resources::MeshType::Quad2),
        m_resources->GetShader(Resources::ShaderType::Grid)
    );
    camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), true, 10.0f);
}

void Scene::AddImage()
{
    images.push_back(std::make_shared<BGImage>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(Resources::TextureType::Default)
    ));
}

void Scene::AddImage(std::string path)
{
    images.push_back(std::make_shared<BGImage>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(path)
    ));
}

void Scene::AddImage(const std::shared_ptr<BGImage>& image)
{
    images.push_back(image);
}

void Scene::AddToken()
{
    tokens.push_back(std::make_shared<Token>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(Resources::TextureType::Default)
    ));
    // Centers it on the camera view
    tokens.back()->GetModel()->SetPos(glm::vec2(camera->Position.x, camera->Position.y));
}

void Scene::AddToken(std::string path)
{
    tokens.push_back(std::make_shared<Token>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(path)
    ));
    // Centers it on the camera view
    tokens.back()->GetModel()->SetPos(glm::vec2(camera->Position.x, camera->Position.y));
}

void Scene::AddToken(const std::shared_ptr<Token>& token)
{
    tokens.push_back(token);
}

void Scene::RemoveOverlay(std::shared_ptr<Overlay> overlay)
{
    auto it = std::find(overlays.begin(), overlays.end(), overlay);
    if (it != overlays.end())
        overlays.erase(it);
}

void Scene::RemoveTokens(std::vector<std::shared_ptr<Token>> toRemove)
{
    auto pred = [&toRemove](const std::shared_ptr<Token>& t) ->bool
    {
        return std::find(toRemove.begin(), toRemove.end(), t) != toRemove.end();
    };

    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), pred), tokens.end());
}

void Scene::RemoveImages(std::vector<std::shared_ptr<BGImage>> toRemove)
{
    auto pred = [&toRemove](const std::shared_ptr<BGImage>& t) ->bool
    {
        return std::find(toRemove.begin(), toRemove.end(), t) != toRemove.end();
    };
    images.erase(std::remove_if(images.begin(), images.end(), pred), images.end());
}

bool Scene::IsEmpty() { return tokens.empty() && images.empty(); }

Bounds2D Scene::GetBounds()
{
    if (IsEmpty())
        return Bounds2D();
    
    std::vector<std::shared_ptr<Shape2D>> shapes {tokens.size() + images.size()};
    int i = 0;
    for (const auto& token: tokens)
        shapes[i++] = static_cast<std::shared_ptr<Shape2D>>(token);
    for (const auto& image: images)
        shapes[i++] = static_cast<std::shared_ptr<Shape2D>>(image);

    return Bounds2D::BoundsForShapes(shapes);
}

// TODO: Move to renderer class
void Scene::Draw()
{
    glClearColor(bgColor.x * bgColor.w, bgColor.y * bgColor.w, bgColor.z * bgColor.w, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    std::shared_ptr<Shader> imageShader = m_resources->GetShader(Resources::ShaderType::Image);
    imageShader->use();
    for (const std::shared_ptr<BGImage>& image: images)
        image->Draw(*imageShader);

    grid->Draw();

    auto quad = m_resources->GetMesh(Resources::MeshType::Quad);
    auto statusTexture = m_resources->GetTexture(Resources::TextureType::Status);
    auto xStatusTexture = m_resources->GetTexture(Resources::TextureType::XStatus);
    std::shared_ptr<Shader> tokenShader = m_resources->GetShader(Resources::ShaderType::Token);
    std::shared_ptr<Shader> statusShader = m_resources->GetShader(Resources::ShaderType::Status);
    tokenShader->use();
    for (const std::shared_ptr<Token>& token : tokens)
    {
        tokenShader->use();
        token->Draw(*tokenShader);

        auto statuses = token->GetStatuses();
        if (statuses.any())
        {
            statusShader->use();
            statusTexture->activate(GL_TEXTURE0);
            statusShader->setInt("diffuse", 0);
            glBindTexture(GL_TEXTURE_2D, statusTexture->ID);

            for (unsigned int i=0; i < statuses.size(); i++)
            {
                if (!statuses[i])
                    continue;
                
                glm::mat4 matrix = glm::mat4(1.0f);
                matrix = glm::translate(matrix, glm::vec3(token->GetModel()->GetPos(), 0.0f));
                matrix = glm::rotate(matrix, glm::radians(360.0f * i / statuses.size()), glm::vec3(0, 0, 1));
                matrix = glm::translate(matrix, glm::vec3(0.0f, token->GetModel()->GetScale().y * 0.35f, 0.0f));
                matrix = glm::scale(matrix, glm::vec3(token->GetModel()->GetScalef() * 0.15f));
                statusShader->setMat4("model", matrix);
                statusShader->setFloat4("color", statusColors[i].x, statusColors[i].y, statusColors[i].z, 1.0f);

                quad->Draw(*statusShader);
            }
        }

        if (token->GetXStatus())
        {
            statusShader->use();
            statusShader->setMat4("model", *token->GetModel()->Value());
            xStatusTexture->activate(GL_TEXTURE0);
            statusShader->setInt("diffuse", 0);
            statusShader->setFloat4("color", 1.0f, 1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D, xStatusTexture->ID);
            quad->Draw(*statusShader);
        }
    }

    // Overlays have their own shaders
    for (const std::shared_ptr<Overlay>& overlay : overlays)
        overlay->Draw();
}

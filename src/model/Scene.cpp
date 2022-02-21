#include <algorithm>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <Resources.h>
#include <glutil/Camera.h>
#include <glutil/Shader.h>
#include <model/BGImage.h>
#include <model/Grid.h>
#include <model/Token.h>
#include <model/Scene.h>


Scene::Scene(std::shared_ptr<Resources> resources) : m_resources(resources)
{
    grid = std::make_shared<Grid>(
        m_resources->GetMesh(Resources::MeshType::Quad2),
        m_resources->GetShader(Resources::ShaderType::Grid)
    );
}

void Scene::AddCamera(const std::shared_ptr<Camera>& camera)
{
    cameras.push_back(camera);
    if (views.empty())
        views.emplace(HOST_VIEW, camera);
}

void Scene::AddDefaultCamera()
{
    AddCamera(std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), true, 10.0f));
}

void Scene::AddImage()
{
    images.push_back(std::make_shared<BGImage>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(Resources::TextureType::Default)
    ));
    images.back()->SetVisibilities(m_defaultVisibilities);
}

void Scene::AddImage(std::string path)
{
    images.push_back(std::make_shared<BGImage>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(path)
    ));
    images.back()->SetVisibilities(m_defaultVisibilities);
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
    tokens.back()->SetVisibilities(m_defaultVisibilities);
}

void Scene::AddToken(std::string path)
{
    tokens.push_back(std::make_shared<Token>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(path)
    ));
    tokens.back()->SetVisibilities(m_defaultVisibilities);
}

void Scene::AddToken(const std::shared_ptr<Token>& token)
{
    tokens.push_back(token);
}

void Scene::RemoveOverlay(std::shared_ptr<Shape2D> overlay)
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

bool Scene::RemoveCamera(const std::shared_ptr<Camera>& camera)
{
    auto it = std::find(cameras.begin(), cameras.end(), camera);
    if (it == cameras.end())
        return false;
    cameras.erase(it);

    // When a camera's deleted, if it's being used for a view then point the
    // view to one of the remaining cameras if any exist.
    for (const auto& it2: views)
    {
        if (it2.second == camera)
        {
            if (cameras.empty())
                views[it2.first] = nullptr;
            else
                views[it2.first] = cameras[0];
            break;
        }
    }

    return true;
}

void Scene::SetViewCamera(ViewID id, const std::shared_ptr<Camera>& camera)
{
    auto it = std::find(cameras.begin(), cameras.end(), camera);
    if (it == cameras.end())
    {
        AddCamera(camera);
    }
    views[id] = camera;
}
const std::shared_ptr<Camera>& Scene::GetViewCamera(ViewID id)
{
    return views[id];
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

std::vector<std::shared_ptr<Shape2D>> Scene::ShapesInBounds(glm::vec2 lo, glm::vec2 hi)
{
    std::vector<std::shared_ptr<Shape2D>> shapes;

    if (!GetTokensLocked())
    {
        for (const std::shared_ptr<Token>& token: tokens)
        {
            float radius = token->GetModel()->GetScalef() * 0.5f;
            glm::vec2 tokenPos = token->GetModel()->GetPos();
            if (tokenPos.x + radius > lo.x && tokenPos.x - radius < hi.x
                && tokenPos.y + radius > lo.y && tokenPos.y - radius < hi.y)
            {
                shapes.push_back(static_cast<std::shared_ptr<Shape2D>>(token));
            }
        }
    }

    if (!GetImagesLocked())
    {
        for (const std::shared_ptr<BGImage>& image: images)
        {
            glm::vec2 scale = image->GetModel()->GetScale() * 0.5f;
            glm::vec2 imageMin = image->GetModel()->GetPos() - scale;
            glm::vec2 imageMax = image->GetModel()->GetPos() + scale;
            if (imageMax.x > lo.x && imageMin.x < hi.x && imageMax.y > lo.y && imageMin.y < hi.y)
            {
                shapes.push_back(static_cast<std::shared_ptr<Shape2D>>(image));
            }
        }
    }

    return shapes;
}

bool Scene::GetImagesLocked() { return m_lockImages; }
void Scene::SetImagesLocked(bool locked) { m_lockImages = locked; }
bool Scene::GetTokensLocked() { return m_lockTokens; }
void Scene::SetTokensLocked(bool locked) { m_lockTokens = locked; }

void Scene::Draw(ShapeVisibility visibility)
{
    glClearColor(bgColor.x * bgColor.w, bgColor.y * bgColor.w, bgColor.z * bgColor.w, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    std::shared_ptr<Shader> imageShader = m_resources->GetShader(Resources::ShaderType::Image);
    imageShader->use();
    for (const std::shared_ptr<BGImage>& image: images)
    {
        if (image->HasVisibility(visibility))
            image->Draw(*imageShader);
    }

    grid->Draw();

    auto quad = m_resources->GetMesh(Resources::MeshType::Quad);
    auto statusTexture = m_resources->GetTexture(Resources::TextureType::Status);
    auto xStatusTexture = m_resources->GetTexture(Resources::TextureType::XStatus);
    std::shared_ptr<Shader> tokenShader = m_resources->GetShader(Resources::ShaderType::Token);
    std::shared_ptr<Shader> statusShader = m_resources->GetShader(Resources::ShaderType::Status);
    tokenShader->use();
    for (const std::shared_ptr<Token>& token : tokens)
    {
        if (!token->HasVisibility(visibility))
            continue;
 
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

                float degree = glm::radians(90 - 360.0f * i / statuses.size());
                
                glm::mat4 matrix = glm::mat4(1.0f);
                matrix = glm::translate(matrix, glm::vec3(token->GetModel()->GetPos() + glm::vec2(glm::cos(degree), glm::sin(degree)) * token->GetModel()->GetScale() * 0.35f, 0.0f));
                matrix = glm::scale(matrix, glm::vec3(token->GetModel()->GetScalef() * 0.15f));
                statusShader->setMat4("model", matrix);
                statusShader->setFloat4("color", statusColors[i].x, statusColors[i].y, statusColors[i].z, token->GetOpacity());

                quad->Draw(*statusShader);
            }
        }

        if (token->GetXStatus())
        {
            statusShader->use();
            statusShader->setMat4("model", *token->GetModel()->Value());
            xStatusTexture->activate(GL_TEXTURE0);
            statusShader->setInt("diffuse", 0);
            statusShader->setFloat4("color", 1.0f, 1.0f, 1.0f, token->GetOpacity());
            glBindTexture(GL_TEXTURE_2D, xStatusTexture->ID);
            quad->Draw(*statusShader);
        }
    }

    const std::shared_ptr<Shader>& overlayShader = m_resources->GetShader(Resources::ShaderType::Simple);
    for (const std::shared_ptr<Shape2D>& overlay : overlays)
    {
        if (overlay->HasVisibility(visibility))
            overlay->Draw(*overlayShader);
    }
}

void Scene::SetDefaultVisibilities(ShapeVisibilities visibilities)
{
    m_defaultVisibilities = visibilities;
}

ShapeVisibilities Scene::GetDefaultVisibilities()
{
    return m_defaultVisibilities;
}

bool Scene::GetDefaultVisibility(ShapeVisibility visibility)
{
    return m_defaultVisibilities[(size_t)visibility];
}

#include <iostream>
#include <memory>

#include <glutil/Buffers.h>
#include <model/Scene.h>
#include <view/Window.h>

#include <view/Viewport.h>


Viewport::Viewport(ViewID viewID, unsigned int width, unsigned int height, const char* name, std::shared_ptr<Window> share) :
    Window(width, height, name, share), m_viewID(viewID)
{}


glm::vec2 Viewport::ScreenToWorldPos(float x, float y)
{
    auto const& camera = GetCamera();
    return glm::vec2(
        camera->Position.x + (2 * x / m_width - 1) * (camera->hAperture * camera->Focal),
        camera->Position.y + (2 * (1 - y / m_height) - 1) * (camera->vAperture * camera->Focal)
    );
}

glm::vec2 Viewport::ScreenToWorldOffset(float x, float y)
{
    auto const& camera = GetCamera();
    return glm::vec2(
        (x / m_width) * (camera->hAperture * 2 * camera->Focal),
        (y / m_height) * (camera->vAperture * 2 * camera->Focal)
    );
}

void Viewport::SetScene(const std::shared_ptr<Scene>& scene)
{
    m_scene = scene;
}

void Viewport::SetCamera(const std::shared_ptr<Camera>& camera)
{
    m_scene->SetViewCamera(m_viewID, camera);
}

const std::shared_ptr<Camera>& Viewport::GetCamera()
{
    return m_scene->GetViewCamera(m_viewID);
}

void Viewport::Focus(const Bounds2D& bounds)
{
    // Inverted GL y position. TODO: Change methods so inversion is handled inside these methods
    glm::vec2 ratios = bounds.Size() / (ScreenToWorldPos(m_width, 0) - ScreenToWorldPos(0, m_height));
    auto const& camera = GetCamera();
    camera->Focal *= std::max(ratios.x, ratios.y);
    camera->Focal *= 1.1f;  // padding
    camera->Position = glm::vec3(bounds.Center().x, bounds.Center().y, camera->Position.z);
    camera->RefreshMatrices();
}

void Viewport::OnWindowResized(int width, int height)
{
    Window::OnWindowResized(width, height);
    GetCamera()->SetAperture((float)m_width / (float)m_height);
}

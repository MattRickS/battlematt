#include <memory>

#include <Buffers.h>
#include <Scene.h>
#include <Window.h>

#include <Viewport.h>


Viewport::Viewport(unsigned int width, unsigned int height, std::shared_ptr<Scene> scene, std::shared_ptr<Window> share) :
    Window(width, height, "Viewport", share), m_scene(scene)
{
    // TODO: This should live on a renderer class...
    m_cameraBuffer = std::make_shared<CameraBuffer>();
    RefreshCamera();
}


glm::vec2 Viewport::ScreenToWorldPos(float x, float y)
{
    return glm::vec2(
        m_scene->camera->Position.x + (2 * x / m_width - 1) * (m_scene->camera->hAperture * m_scene->camera->Focal),
        m_scene->camera->Position.y + (2 * (1 - y / m_height) - 1) * (m_scene->camera->vAperture * m_scene->camera->Focal)
    );
}

glm::vec2 Viewport::ScreenToWorldOffset(float x, float y)
{
    return glm::vec2(
        (x / m_width) * (m_scene->camera->hAperture * 2 * m_scene->camera->Focal),
        (y / m_height) * (m_scene->camera->vAperture * 2 * m_scene->camera->Focal)
    );
}

void Viewport::SetScene(std::shared_ptr<Scene> scene)
{
    m_scene = scene;
    RefreshCamera();
}
void Viewport::RefreshCamera()
{
    m_scene->camera->SetAperture((float)m_width / (float)m_height);
    m_cameraBuffer->SetCamera(m_scene->camera);
}

void Viewport::Draw()
{
    // TODO: Move drawing logic out of scene/other classes and into this class.
    m_scene->Draw();
}

void Viewport::OnWindowResized(int width, int height)
{
    Window::OnWindowResized(width, height);
    m_scene->camera->SetAperture((float)m_width / (float)m_height);
}

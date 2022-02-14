#include <iostream>
#include <memory>

#include <glutil/Buffers.h>
#include <model/Scene.h>
#include <view/Window.h>

#include <view/Viewport.h>


Viewport::Viewport(unsigned int width, unsigned int height, std::shared_ptr<Window> share) :
    Window(width, height, "Viewport", share)
{
    // TODO: This should live on a renderer class...
    m_cameraBuffer = std::make_shared<CameraBuffer>();
}


glm::vec2 Viewport::ScreenToWorldPos(float x, float y)
{
    return glm::vec2(
        m_camera->Position.x + (2 * x / m_width - 1) * (m_camera->hAperture * m_camera->Focal),
        m_camera->Position.y + (2 * (1 - y / m_height) - 1) * (m_camera->vAperture * m_camera->Focal)
    );
}

glm::vec2 Viewport::ScreenToWorldOffset(float x, float y)
{
    return glm::vec2(
        (x / m_width) * (m_camera->hAperture * 2 * m_camera->Focal),
        (y / m_height) * (m_camera->vAperture * 2 * m_camera->Focal)
    );
}

void Viewport::SetScene(std::shared_ptr<Scene> scene, int cameraIndex)
{
    m_scene = scene;
    m_camera = m_scene->cameras[cameraIndex];
    RefreshCamera();
}

void Viewport::SetCamera(const std::shared_ptr<Camera>& camera)
{
    m_camera = camera;
    RefreshCamera();
}

const std::shared_ptr<Camera>& Viewport::GetCamera()
{
    return m_camera;
}

void Viewport::RefreshCamera()
{
    // TODO: Move this aperture setting out to Controller window resizing
    m_camera->SetAperture((float)m_width / (float)m_height);
    m_cameraBuffer->SetCamera(m_camera);
}

void Viewport::Focus(const Bounds2D& bounds)
{
    // Inverted GL y position. TODO: Change methods so inversion is handled inside these methods
    glm::vec2 ratios = bounds.Size() / (ScreenToWorldPos(m_width, 0) - ScreenToWorldPos(0, m_height));
    m_camera->Focal *= std::max(ratios.x, ratios.y);
    m_camera->Focal *= 1.1f;  // padding
    m_camera->Position = glm::vec3(bounds.Center().x, bounds.Center().y, m_camera->Position.z);
    m_camera->RefreshMatrices();
    RefreshCamera();
}

void Viewport::Draw()
{
    // The current design assumes the viewport can be varied, but is limited to
    // a single GL context (and therefore a single camera). Hack for now to ensure
    // the primary camera is always rendered even if undo/redo changes the current
    // camera. Undo/Redo needs a better way of informing the Renderer the buffer
    // must be reloaded.
    if (m_camera != m_scene->GetViewCamera(HOST_VIEW))
    {
        std::cerr << "Camera buffer out of sync with scene, refreshing" << std::endl;
        m_camera = m_scene->GetViewCamera(HOST_VIEW);
        RefreshCamera();
    }
    // TODO: Move drawing logic out of scene/other classes and into this class.
    m_scene->Draw();
}

void Viewport::OnWindowResized(int width, int height)
{
    Window::OnWindowResized(width, height);
    m_camera->SetAperture((float)m_width / (float)m_height);
}

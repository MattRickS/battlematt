#include <memory>

#include <glutil/Buffers.h>
#include <model/Scene.h>
#include <view/Window.h>

#include <view/Viewport.h>


Viewport::Viewport(unsigned int width, unsigned int height, std::string name, std::shared_ptr<Window> share) :
    Window(width, height, name.c_str(), share)
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

std::vector<std::shared_ptr<Shape2D>> Viewport::ShapesInScreenRect(float minx, float miny, float maxx, float maxy)
{
    glm::vec2 lo = ScreenToWorldPos(minx, miny);
    glm::vec2 hi = ScreenToWorldPos(maxx, maxy);

    std::vector<std::shared_ptr<Shape2D>> shapes;

    if (!m_scene->GetTokensLocked())
    {
        for (const std::shared_ptr<Token>& token: m_scene->tokens)
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

    if (!m_scene->GetImagesLocked())
    {
        for (const std::shared_ptr<BGImage>& image: m_scene->images)
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

std::vector<std::shared_ptr<Shape2D>> Viewport::ShapesInScreenRect(Bounds2D rect)
{
    return ShapesInScreenRect(rect.MinX(), rect.MinY(), rect.MaxX(), rect.MaxY());
}

std::shared_ptr<Shape2D> Viewport::GetShapeAtScreenPos(glm::vec2 screenPos)
{
    glm::vec2 worldPos = ScreenToWorldPos(screenPos.x, screenPos.y);
    // Tokens are drawn from first to last, so iterate in reverse to find the topmost
    if (!m_scene->GetTokensLocked())
    {
        for (int i = m_scene->tokens.size() - 1; i >= 0; i--)
        {
            // It should only be possible to select one shape with a single click
            if (m_scene->tokens[i]->Contains(worldPos))
            {
                return static_cast<std::shared_ptr<Shape2D>>(m_scene->tokens[i]);
            }
        }
    }
    // Images are drawn from first to last, so iterate in reverse to find the topmost
    if (!m_scene->GetImagesLocked())
    {
        for (int i = m_scene->images.size() - 1; i >= 0; i--)
        {
            // It should only be possible to select one shape with a single click
            if (m_scene->images[i]->Contains(worldPos))
            {
                return static_cast<std::shared_ptr<Shape2D>>(m_scene->images[i]);
            }
        }
    }
    return nullptr;
}

// Drag Selection
bool Viewport::IsDragSelecting()
{
    return static_cast<bool>(dragSelectRect);
}

void Viewport::StartDragSelection(const std::shared_ptr<RectOverlay>& rect, float xpos, float ypos)
{
    dragSelectRect = rect;
    // GL uses inverted Y-axis
    dragSelectRect->startCorner = dragSelectRect->endCorner = glm::vec2(xpos, Height() - ypos);
}

void Viewport::UpdateDragSelection(float xpos, float ypos)
{
    // GL uses inverted Y-axis
    dragSelectRect->endCorner = glm::vec2(xpos, Height() - ypos);

    // Y-axis is inverted on rect, use re-invert for calculating world positions
    auto coveredShapes = ShapesInScreenRect(
        dragSelectRect->MinX(),
        Height() - dragSelectRect->MinY(),
        dragSelectRect->MaxX(),
        Height() - dragSelectRect->MaxY()
    );

    for (const std::shared_ptr<Shape2D>& shape : coveredShapes)
        shape->isHighlighted = true;
}

Bounds2D Viewport::DragSelectionRect()
{
    // TODO: RectOverlay should use bounds
    return Bounds2D(
        dragSelectRect->MinX(),
        Height() - dragSelectRect->MinY(),
        dragSelectRect->MaxX(),
        Height() - dragSelectRect->MaxY()
    );
}

void Viewport::FinishDragSelection()
{
    dragSelectRect.reset();
}

void Viewport::SetScene(std::shared_ptr<Scene> scene, int cameraIndex)
{
    m_scene = scene;
    m_camera = m_scene->cameras[cameraIndex];
    RefreshCamera();
}

std::shared_ptr<Camera>& Viewport::GetCamera()
{
    return m_camera;
}

int Viewport::GetCameraIndex()
{
    auto it = std::find(m_scene->cameras.begin(), m_scene->cameras.end(), m_camera);
    return it - m_scene->cameras.begin();
}

void Viewport::SetCameraIndex(int index)
{
    m_camera = m_scene->cameras[index];
    RefreshCamera();
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
    // TODO: Move drawing logic out of scene/other classes and into this class.
    m_scene->Draw();
    if (dragSelectRect)
        dragSelectRect->Draw();
}

void Viewport::OnWindowResized(int width, int height)
{
    Window::OnWindowResized(width, height);
    m_camera->SetAperture((float)m_width / (float)m_height);
}

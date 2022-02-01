#pragma once
#include <memory>

#include <glutil/Buffers.h>
#include <model/Scene.h>
#include <view/Window.h>


class Viewport : public Window
{
public:
    Viewport(unsigned int width, unsigned int height, std::string name="Viewport", std::shared_ptr<Window> share = NULL);
    // ~Viewport();

    virtual void Draw();
    void SetScene(std::shared_ptr<Scene> scene, int cameraIndex=0);
    std::shared_ptr<Camera>& GetCamera();
    int GetCameraIndex();
    void SetCameraIndex(int index);

    // Orthographic operations (Matrices aren't working correctly if camera position is changed)
    glm::vec2 ScreenToWorldPos(float x, float y);
    glm::vec2 ScreenToWorldOffset(float x, float y);
    std::vector<std::shared_ptr<Shape2D>> ShapesInScreenRect(float minx, float miny, float maxx, float maxy);
    std::vector<std::shared_ptr<Shape2D>> ShapesInScreenRect(Bounds2D bounds);
    std::shared_ptr<Shape2D> GetShapeAtScreenPos(glm::vec2 screenPos);

    bool IsDragSelecting();
    // TODO: Avoid passing this in. Needs resources to init, resources needs a window to exist first.
    void StartDragSelection(const std::shared_ptr<RectOverlay>& rect, float xpos, float ypos);
    void UpdateDragSelection(float xpos, float ypos);
    Bounds2D DragSelectionRect();
    void FinishDragSelection();

    void RefreshCamera();
    void Focus(const Bounds2D& bounds);

    virtual void OnWindowResized(int width, int height);
protected:
    std::shared_ptr<Scene> m_scene = nullptr;
    std::shared_ptr<Camera> m_camera = nullptr;
    std::shared_ptr<CameraBuffer> m_cameraBuffer = nullptr;

    std::shared_ptr<RectOverlay> dragSelectRect = nullptr;
};

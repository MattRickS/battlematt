#pragma once
#include <memory>

#include <glutil/Buffers.h>
#include <model/Scene.h>
#include <view/Window.h>


class Viewport : public Window
{
public:
    Viewport(ViewID viewID, unsigned int width, unsigned int height, const char* name, std::shared_ptr<Window> share = NULL);

    void SetScene(const std::shared_ptr<Scene>& scene);
    void SetCamera(const std::shared_ptr<Camera>& camera);
    const std::shared_ptr<Camera>& GetCamera();

    // Orthographic operations (Matrices aren't working correctly if camera position is changed)
    glm::vec2 ScreenToWorldPos(float x, float y);
    glm::vec2 ScreenToWorldOffset(float x, float y);

    void Focus(const Bounds2D& bounds);

    virtual void OnWindowResized(int width, int height);
    std::shared_ptr<Scene> m_scene = nullptr;
private:
    ViewID m_viewID;
};

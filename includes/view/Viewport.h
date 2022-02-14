#pragma once
#include <memory>

#include <glutil/Buffers.h>
#include <model/Scene.h>
#include <view/Window.h>


class Viewport : public Window
{
public:
    Viewport(unsigned int width, unsigned int height, const char* name, std::shared_ptr<Window> share = NULL);
    // ~Viewport();

    virtual void Draw();
    void SetScene(std::shared_ptr<Scene> scene, int cameraIndex=0);
    void SetCamera(const std::shared_ptr<Camera>& camera);
    const std::shared_ptr<Camera>& GetCamera();

    // Orthographic operations (Matrices aren't working correctly if camera position is changed)
    glm::vec2 ScreenToWorldPos(float x, float y);
    glm::vec2 ScreenToWorldOffset(float x, float y);

    void RefreshCamera();
    void Focus(const Bounds2D& bounds);

    virtual void OnWindowResized(int width, int height);
    std::shared_ptr<Scene> m_scene = nullptr;
private:
    std::shared_ptr<Camera> m_camera = nullptr;
    std::shared_ptr<CameraBuffer> m_cameraBuffer = nullptr;
};

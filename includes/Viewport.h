#pragma once
#include <memory>

#include <Buffers.h>
#include <Scene.h>
#include <Window.h>


class Viewport : public Window
{
public:
    Viewport(unsigned int width, unsigned int height, std::shared_ptr<Scene> scene);
    ~Viewport();

    virtual void Draw();
    void SetScene(std::shared_ptr<Scene> scene);

    // Orthographic operations (Matrices aren't working correctly if camera position is changed)
    glm::vec2 ScreenToWorldPos(float x, float y);
    glm::vec2 ScreenToWorldOffset(float x, float y);

    void RefreshCamera();

    virtual void OnWindowResized(int width, int height);
private:
    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<CameraBuffer> m_cameraBuffer = nullptr;
};

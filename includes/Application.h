#pragma once
#include <memory>

#include <Buffers.h>
#include <InputManager.h>
#include <JSONSerializer.h>
#include <Resources.h>
#include <Scene.h>
#include <UIWindow.h>
#include <Window.h>


class Application
{
public:
    const char* glsl_version = "#version 330";

    Application();
    ~Application();

    void LoadDefaultResources();
    bool IsInitialised();
    void Draw();
    
    void Save(std::string path);
    void Load(std::string path);

private:
    bool m_glfw_initialised = false;
    bool m_glad_initialised = false;

    float deltaTime, lastFrame = 0.0f;
    std::shared_ptr<CameraBuffer> cameraBuffer = nullptr;  // Where should this live? Should there be a Renderer class? A Viewport class?
    std::shared_ptr<Resources> m_resources;
    JSONSerializer m_serializer;
    std::shared_ptr<Scene> m_scene = nullptr;
    std::shared_ptr<InputManager> m_inputManager = nullptr;
    std::shared_ptr<Window> m_viewport = nullptr;
    std::shared_ptr<UIWindow> m_uiWindow = nullptr;

    void InitGL();
};

#pragma once
#include <memory>

#include <Buffers.h>
#include <Controller.h>
#include <JSONSerializer.h>
#include <Resources.h>
#include <Scene.h>
#include <UIWindow.h>
#include <Viewport.h>


class Application
{
public:
    const char* glsl_version = "#version 330";

    Application();
    ~Application();

    bool IsInitialised();
    void Exec();
    
    void Save(std::string path);
    void Load(std::string path);

private:
    bool m_glfw_initialised = false;

    std::shared_ptr<Resources> m_resources = nullptr;
    JSONSerializer m_serializer;
    std::shared_ptr<Scene> m_scene = nullptr;
    std::shared_ptr<Controller> m_controller = nullptr;
    std::shared_ptr<Viewport> m_viewport = nullptr;
    std::shared_ptr<UIWindow> m_uiWindow = nullptr;

    void LoadDefaultResources();
};

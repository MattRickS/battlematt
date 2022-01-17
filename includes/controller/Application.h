#pragma once
#include <memory>

#include <Controller.h>
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

private:
    bool m_glfw_initialised = false;

    std::shared_ptr<Resources> m_resources = nullptr;
    std::shared_ptr<Controller> m_controller = nullptr;
    std::shared_ptr<Viewport> m_viewport = nullptr;
    std::shared_ptr<UIWindow> m_uiWindow = nullptr;

    void LoadDefaultResources();
};

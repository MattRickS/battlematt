#pragma once
#include <memory>

#include <Resources.h>
#include <controller/Controller.h>
#include <model/Scene.h>
#include <view/UIWindow.h>
#include <view/Viewport.h>


class Application
{
public:
    const char* glsl_version = "#version 330";
    std::shared_ptr<Controller> controller = nullptr;

    Application();
    ~Application();

    bool IsInitialised();
    void Exec();

private:
    bool m_glfw_initialised = false;

    std::shared_ptr<Resources> m_resources = nullptr;
    std::shared_ptr<Viewport> m_viewport = nullptr;
    std::shared_ptr<UIWindow> m_uiWindow = nullptr;

    void LoadDefaultResources();
};

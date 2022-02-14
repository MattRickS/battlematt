#pragma once
#include <memory>

#include <GLFW/glfw3.h>

#include <Resources.h>
#include <controller/Controller.h>
#include <model/Scene.h>
#include <view/UIWindow.h>
#include <view/Viewport.h>
// #include <view/SharedRenderBuffer.h>


class Application
{
public:
    const char* glsl_version = "#version 460";
    std::shared_ptr<Controller> controller = nullptr;

    Application();
    ~Application();

    bool IsInitialised();
    void Exec();

private:
    bool m_glfw_initialised = false;

    std::shared_ptr<Resources> m_resources = nullptr;
    std::shared_ptr<Viewport> m_presentationWindow = nullptr;
    std::shared_ptr<UIWindow> m_hostWindow = nullptr;

    void LoadDefaultResources();
};

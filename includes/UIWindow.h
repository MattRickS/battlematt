#pragma once
#include <memory>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Scene.h>
#include <Token.h>
#include <Overlays.h>
#include <Window.h>


class UIWindow : public Window
{
public:
    UIWindow(unsigned int width, unsigned int height, const char* glsl_version, const char* name);
    ~UIWindow();

    virtual void Draw(std::shared_ptr<Scene> scene);
};

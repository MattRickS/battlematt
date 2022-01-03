#pragma once
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Scene.h>
#include <Token.h>


struct UIState
{
    bool snapToGrid = false;
    std::vector<Token*> selectedTokens;
};


class ImGuiContextGuard
{
public:
    ImGuiContextGuard(GLFWwindow* window, const char* glsl_version);
    ~ImGuiContextGuard();
};

bool FileLine(std::string label, std::string& path);

void DrawUI(Scene* scene, UIState* uiState);

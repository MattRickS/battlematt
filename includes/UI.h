#pragma once
#include <memory>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Scene.h>
#include <Token.h>
#include <Overlays.h>


struct UIState
{
    bool snapToGrid = false;
    std::vector<Token*> selectedTokens;
    std::unique_ptr<RectOverlay> dragSelectRect;
    Token* tokenUnderCursor = nullptr;
};


class ImGuiContextGuard
{
public:
    ImGuiContextGuard(GLFWwindow* window, const char* glsl_version);
    ~ImGuiContextGuard();
};

bool FileLine(std::string label, std::string& path);

void DrawUI(Scene* scene, UIState* uiState);

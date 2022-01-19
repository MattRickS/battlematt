#pragma once
#include <deque>
#include <memory>
#include <vector>

#include <Actions.hpp>
#include <JSONSerializer.h>
#include <Resources.h>
#include <model/Overlays.h>
#include <model/Scene.h>
#include <model/Token.h>
#include <view/UIState.h>
#include <view/UIWindow.h>
#include <view/Viewport.h>


class Controller
{
public:
    std::shared_ptr<UIState> uiState = std::make_shared<UIState>();

    Controller(std::shared_ptr<Resources> resources, std::shared_ptr<Viewport> viewport, std::shared_ptr<UIWindow> uiWindow);

    void SetScene(std::shared_ptr<Scene> scene);
    void Save(std::string path);
    void Load(std::string path, bool merge);

    std::vector<std::shared_ptr<Token>> SelectedTokens();
    bool HasSelectedTokens();
    void ClearSelection();
    void SelectToken(std::shared_ptr<Token> token, bool additive=true);
    void DuplicateSelectedTokens();

    std::vector<std::shared_ptr<Token>> TokensInScreenRect(float minx, float miny, float maxx, float maxy);
    std::shared_ptr<Token> GetTokenAtScreenPos(glm::vec2 screenPos);

    // Have to public for the glfw bound callbacks to access
    void OnViewportMouseMove(double xpos, double ypos);
    void OnViewportMouseButton(int button, int action, int mods);
    void OnViewportMouseScroll(double xoffset, double yoffset);
    void OnViewportKey(int key, int scancode, int action, int mods);
    void OnViewportSizeChanged(int width, int height);
    void OnCloseRequested();

    void OnUIAddTokenClicked();
    void OnUIKeyChanged(int key, int scancode, int action, int mods);
    void PerformAction(std::shared_ptr<Action> action);

    bool Undo();
    bool Redo();

private:
    std::shared_ptr<Resources> m_resources = nullptr;
    std::shared_ptr<Scene> m_scene = nullptr;
    std::shared_ptr<Viewport> m_viewport = nullptr;
    std::shared_ptr<UIWindow> m_uiWindow = nullptr;
    JSONSerializer m_serializer;

    bool firstMouse = true;
    float lastMouseX, lastMouseY;
    bool middleMouseHeld = false;
    bool leftMouseHeld = false;

    const size_t MAX_UNDO_SIZE = 10;
    std::deque<std::shared_ptr<Action>> undoQueue;
    std::deque<std::shared_ptr<Action>> redoQueue;

    bool IsDragSelecting();
    void StartDragSelection(float xpos, float ypos);
    void UpdateDragSelection(float xpos, float ypos);
    void FinishDragSelection();

    static const int PROMPT_CLOSE = 1;
    void OnPromptResponse(int promptType, bool response);
};

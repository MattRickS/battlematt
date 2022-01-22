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
#include <view/Properties.h>
#include <view/UIWindow.h>
#include <view/Viewport.h>


class Controller
{
public:
    Controller(std::shared_ptr<Resources> resources, std::shared_ptr<Viewport> viewport, std::shared_ptr<UIWindow> uiWindow);
    ~Controller();

    void SetScene(std::shared_ptr<Scene> scene);
    void Save(std::string path);
    void Load(std::string path, bool merge);

    std::vector<std::shared_ptr<Token>> SelectedTokens();
    bool HasSelectedTokens();
    void ClearSelection();
    void SelectToken(std::shared_ptr<Token> token, bool additive=false);
    void SelectTokens(std::vector<std::shared_ptr<Token>> tokens, bool additive=false);
    void DuplicateSelectedTokens();
    void DeleteSelectedTokens();

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
    void PerformAction(const std::shared_ptr<Action>& action);

    bool Undo();
    bool Redo();

    void OnTokenPropertyChanged(const std::shared_ptr<Token>& token, TokenProperty property, TokenPropertyValue value);
    void OnImagePropertyChanged(const std::shared_ptr<BGImage>& image, ImageProperty property, ImagePropertyValue value);
    void OnGridPropertyChanged(const std::shared_ptr<Grid>& grid, GridProperty property, GridPropertyValue value);

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

    std::shared_ptr<RectOverlay> dragSelectRect = nullptr;
    std::shared_ptr<Token> tokenUnderCursor = nullptr;

    bool IsDragSelecting();
    void StartDragSelection(float xpos, float ypos);
    void UpdateDragSelection(float xpos, float ypos);
    void FinishDragSelection(bool additive);

    static const int PROMPT_CLOSE = 1;
    void OnPromptResponse(int promptType, bool response);
};

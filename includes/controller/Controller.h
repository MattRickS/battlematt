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
    void Load(std::string path, bool merge = false);
    void Merge(const std::shared_ptr<Scene>& scene);

    void SetImagesLocked(bool locked);
    void SetTokensLocked(bool locked);

    // TODO: Image needs to render selection highlight
    std::vector<std::shared_ptr<Shape2D>> SelectedShapes();
    std::vector<std::shared_ptr<Token>> SelectedTokens();
    std::vector<std::shared_ptr<BGImage>> SelectedImages();
    bool HasSelectedImages();
    bool HasSelectedTokens();
    bool HasSelectedShapes();
    void ClearSelection();
    void SelectShape(std::shared_ptr<Shape2D> shape, bool additive=false);
    // void SelectShape(const std::shared_ptr<Shape2D>& shape, bool additive=false);  // Clashes with signal.
    void SelectShapes(const std::vector<std::shared_ptr<Shape2D>>& shapes, bool additive=false);

    bool CopySelected();
    void CutSelected();
    void PasteSelected();
    void DuplicateSelected();
    void DeleteSelected();

    void Focus();
    void FocusSelected();

    void CloneCamera();
    void SetHostCameraIndex(int index);

    std::vector<std::shared_ptr<Shape2D>> ShapesInScreenRect(float minx, float miny, float maxx, float maxy);
    std::shared_ptr<Shape2D> GetShapeAtScreenPos(glm::vec2 screenPos);

    // Have to public for the glfw bound callbacks to access
    void OnViewportMouseMove(double xpos, double ypos);
    void OnViewportMouseButton(int button, int action, int mods);
    void OnViewportMouseScroll(double xoffset, double yoffset);
    void OnViewportKey(int key, int scancode, int action, int mods);
    void OnViewportSizeChanged(int width, int height);
    void OnCloseRequested();

    void OnUIAddTokenClicked();
    void OnUIAddImageClicked();
    void OnUIRemoveImageClicked(const std::shared_ptr<BGImage>& image);
    void OnUIKeyChanged(int key, int scancode, int action, int mods);
    void PerformAction(const std::shared_ptr<Action>& action);

    bool Undo();
    bool Redo();

    void OnTokenPropertyChanged(const std::shared_ptr<Token>& token, TokenProperty property, TokenPropertyValue value);
    void OnImagePropertyChanged(const std::shared_ptr<BGImage>& image, ImageProperty property, ImagePropertyValue value);
    void OnGridPropertyChanged(const std::shared_ptr<Grid>& grid, GridProperty property, GridPropertyValue value);
    void OnCameraPropertyChanged(const std::shared_ptr<Camera>& camera, CameraProperty propert, CameraPropertyValue value);

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
    std::shared_ptr<Shape2D> shapeUnderCursor = nullptr;

    bool IsDragSelecting();
    void StartDragSelection(float xpos, float ypos);
    void UpdateDragSelection(float xpos, float ypos);
    void FinishDragSelection(bool additive);

    static const int PROMPT_CLOSE = 1;
    void OnPromptResponse(int promptType, bool response);

    // Shared key change behaviour
    void OnKeyChanged(int key, int scancode, int action, int mods);
};

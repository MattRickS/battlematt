#pragma once
#include <deque>
#include <memory>
#include <vector>

#include <Actions.hpp>
#include <JSONSerializer.h>
#include <Resources.h>
#include <glutil/Buffers.h>
#include <model/Shape2D.h>
#include <model/Scene.h>
#include <model/Token.h>
#include <view/Properties.h>
#include <view/UIControls.h>
#include <view/Viewport.h>


class Controller
{
public:
    Controller(
        const std::shared_ptr<Resources>& resources,
        const std::shared_ptr<Viewport>& presentationWindow,
        const std::shared_ptr<Viewport>& hostWindow,
        const std::shared_ptr<UIControls>& uiControls
    );
    ~Controller();

    void Render();

    void SetScene(std::shared_ptr<Scene> scene);
    void Save(std::string path);
    void Load(std::string path, bool merge = false);
    void Merge(const std::shared_ptr<Scene>& scene);

    void SetImagesLocked(bool locked);
    void SetTokensLocked(bool locked);
    void SetDefaultVisibilities(ShapeVisibilities visibilities);

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
    void DeleteCamera();
    void SetHostCamera(const std::shared_ptr<Camera>& camera);

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
    void PerformAction(const std::shared_ptr<Action>& action);

    bool Undo();
    bool Redo();

    void OnTokenPropertyChanged(const std::shared_ptr<Token>& token, TokenProperty property, TokenPropertyValue value);
    void OnImagePropertyChanged(const std::shared_ptr<BGImage>& image, ImageProperty property, ImagePropertyValue value);
    void OnGridPropertyChanged(const std::shared_ptr<Grid>& grid, GridProperty property, GridPropertyValue value);
    void OnCameraPropertyChanged(const std::shared_ptr<Camera>& camera, CameraProperty property, CameraPropertyValue value);

private:
    std::shared_ptr<Resources> m_resources = nullptr;
    std::shared_ptr<Scene> m_scene = nullptr;
    std::shared_ptr<Viewport> m_presentationWindow = nullptr;
    std::shared_ptr<Viewport> m_hostWindow = nullptr;
    std::shared_ptr<UIControls> m_uiControls = nullptr;
    JSONSerializer m_serializer;

    GLuint renderbuffer, hostFramebuffer, presentationFramebuffer;
    std::shared_ptr<CameraBuffer> m_cameraBuffer = nullptr;
    void SetupBuffers();

    bool isPresentationActive = true;
    void RenderHost();
    void RenderPresentation();

    const std::shared_ptr<Viewport>& ActiveViewport();
    const std::shared_ptr<Camera>& ActiveCamera();

    bool firstMouse = true;
    float lastMouseX, lastMouseY;
    bool middleMouseHeld = false;
    bool leftMouseHeld = false;

    const size_t MAX_UNDO_SIZE = 10;
    std::deque<std::shared_ptr<Action>> undoQueue;
    std::deque<std::shared_ptr<Action>> redoQueue;

    std::shared_ptr<Rect> dragSelectRect = nullptr;
    std::shared_ptr<Shape2D> shapeUnderCursor = nullptr;
    bool IsDragSelecting();
    void StartDragSelection(float xpos, float ypos);
    void UpdateDragSelection(float xpos, float ypos);
    void FinishDragSelection(bool additive);

    std::vector<std::shared_ptr<Shape2D>> ShapesInScreenRect(const std::shared_ptr<Viewport>& viewport, float minx, float miny, float maxx, float maxy);
    std::shared_ptr<Shape2D> GetShapeAtScreenPos(const std::shared_ptr<Viewport>& viewport, glm::vec2 screenPos);

    static const int PROMPT_CLOSE = 1;
    void OnPromptResponse(int promptType, bool response);
};

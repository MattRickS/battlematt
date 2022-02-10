#include <fstream>
#include <iostream>
#include <memory>
#include <variant>
#include <vector>

#include <Constants.h>
#include <JSONSerializer.h>
#include <Resources.h>
#include <model/Overlays.h>
#include <model/Scene.h>
#include <model/Token.h>
#include <view/UIWindow.h>
#include <view/Viewport.h>

#include <controller/Controller.h>


Controller::Controller(std::shared_ptr<Resources> resources, std::shared_ptr<Viewport> viewport, std::shared_ptr<UIWindow> uiWindow) :
    m_resources(resources), m_viewport(viewport), m_uiWindow(uiWindow), m_serializer(m_resources)
{
    m_viewport->cursorMoved.connect(this, &Controller::OnViewportMouseMove);
    m_viewport->keyChanged.connect(this, &Controller::OnViewportKey);
    m_viewport->mouseButtonChanged.connect(this, &Controller::OnViewportMouseButton);
    m_viewport->mouseScrolled.connect(this, &Controller::OnViewportMouseScroll);
    m_viewport->sizeChanged.connect(this, &Controller::OnViewportSizeChanged);
    m_viewport->closeRequested.connect(this, &Controller::OnCloseRequested);

    m_uiWindow->saveClicked.connect(this, &Controller::Save);
    m_uiWindow->loadClicked.connect(this, &Controller::Load);
    m_uiWindow->promptResponse.connect(this, &Controller::OnPromptResponse);
    m_uiWindow->shapeSelectionChanged.connect(this, &Controller::SelectShape);
    m_uiWindow->closeRequested.connect(this, &Controller::OnCloseRequested);
    m_uiWindow->keyChanged.connect(this, &Controller::OnUIKeyChanged);
    m_uiWindow->tokenPropertyChanged.connect(this, &Controller::OnTokenPropertyChanged);
    m_uiWindow->imagePropertyChanged.connect(this, &Controller::OnImagePropertyChanged);
    m_uiWindow->gridPropertyChanged.connect(this, &Controller::OnGridPropertyChanged);
    m_uiWindow->cameraPropertyChanged.connect(this, &Controller::OnCameraPropertyChanged);
    m_uiWindow->addTokenClicked.connect(this, &Controller::OnUIAddTokenClicked);
    m_uiWindow->addImageClicked.connect(this, &Controller::OnUIAddImageClicked);
    m_uiWindow->removeImageClicked.connect(this, &Controller::OnUIRemoveImageClicked);
    m_uiWindow->imageLockChanged.connect(this, &Controller::SetImagesLocked);
    m_uiWindow->tokenLockChanged.connect(this, &Controller::SetTokensLocked);
    m_uiWindow->cameraSelectionChanged.connect(this, &Controller::SetHostCamera);
    m_uiWindow->cloneCameraClicked.connect(this, &Controller::CloneCamera);
    m_uiWindow->deleteCameraClicked.connect(this, &Controller::DeleteCamera);

    SetScene(std::make_shared<Scene>(m_resources));
}

Controller::~Controller()
{
    undoQueue.clear();
    redoQueue.clear();
}

// Scene Management
void Controller::SetScene(std::shared_ptr<Scene> scene)
{
    // A scene must have at least one camera to be rendered
    if (scene->cameras.empty())
        scene->AddDefaultCamera();

    m_scene = scene;
    m_viewport->SetScene(scene);
    m_uiWindow->SetScene(scene);
    undoQueue.clear();
    redoQueue.clear();
}

void Controller::Save(std::string path)
{
    std::cerr << "Saving to " << path << std::endl;
    std::ofstream myfile (path);
    if (myfile.is_open())
    {
        myfile << m_serializer.SerializeScene(m_scene);
        myfile.close();
        m_scene->sourceFile = path;
    }
    else
        std::cerr << "Unable to open file" << std::endl;
}

void Controller::Load(std::string path, bool merge)
{
    std::cerr << "Loading Scene from " << path << std::endl;
    nlohmann::json j;
    std::ifstream myfile (path);
    if (myfile.is_open())
    {
        myfile >> j;
        myfile.close();
        std::shared_ptr<Scene> scene = std::make_shared<Scene>(m_resources);
        if (merge)
        {
            m_serializer.DeserializeScene(j, *scene);
            Merge(scene);
        }
        else
        {
            m_serializer.DeserializeScene(j, *scene);
            scene->sourceFile = path;
            SetScene(scene);
        }
    }
    else
        std::cerr << "Unable to open file" << std::endl;
}

void Controller::Merge(const std::shared_ptr<Scene>& scene)
{
    std::shared_ptr<ActionGroup> actionGroup = std::make_shared<ActionGroup>();
    std::vector<std::shared_ptr<Shape2D>> shapes;

    if (!scene->tokens.empty())
    {
        actionGroup->Add(std::make_shared<AddTokensAction>(m_scene, scene->tokens));
        for (const auto& token: scene->tokens)
            shapes.push_back(static_cast<std::shared_ptr<Shape2D>>(token));
    }

    if (!scene->images.empty())
    {
        actionGroup->Add(std::make_shared<AddImagesAction>(m_scene, scene->images));
        for (const auto& image: scene->images)
            shapes.push_back(static_cast<std::shared_ptr<Shape2D>>(image));
    }

    if (!actionGroup->IsEmpty())
    {
        actionGroup->Add(std::make_shared<SelectShapesAction>(SelectedShapes(), shapes));
        PerformAction(actionGroup);
    }
}

// Locking
void Controller::SetImagesLocked(bool locked)
{
    std::shared_ptr<ActionGroup> actionGroup = std::make_shared<ActionGroup>();
    actionGroup->Add(std::make_shared<ModifySceneLocks>(m_scene, &Scene::SetImagesLocked, m_scene->GetImagesLocked(), locked));

    // Deselect images
    if (HasSelectedImages())
    {
        auto selectedShapes = SelectedShapes();
        std::vector<std::shared_ptr<Shape2D>> newSelectedShapes;
        std::copy_if(selectedShapes.begin(), selectedShapes.end(), std::back_inserter(newSelectedShapes),
                     [](const auto& shape){ return !std::dynamic_pointer_cast<BGImage>(shape);});
        actionGroup->Add(std::make_shared<SelectShapesAction>(selectedShapes, newSelectedShapes));
    }
    PerformAction(actionGroup);
}

void Controller::SetTokensLocked(bool locked)
{
    std::shared_ptr<ActionGroup> actionGroup = std::make_shared<ActionGroup>();
    actionGroup->Add(std::make_shared<ModifySceneLocks>(m_scene, &Scene::SetTokensLocked, m_scene->GetTokensLocked(), locked));

    // Deselect tokens
    if (HasSelectedTokens())
    {
        auto selectedShapes = SelectedShapes();
        std::vector<std::shared_ptr<Shape2D>> newSelectedShapes;
        std::copy_if(selectedShapes.begin(), selectedShapes.end(), std::back_inserter(newSelectedShapes),
                     [](const auto& shape){ return !std::dynamic_pointer_cast<Token>(shape);});
        actionGroup->Add(std::make_shared<SelectShapesAction>(selectedShapes, newSelectedShapes));
    }
    PerformAction(actionGroup);
}

// Selection
std::vector<std::shared_ptr<Shape2D>> Controller::SelectedShapes()
{
    // TODO: Could this return an iterator? would be nicer.
    std::vector<std::shared_ptr<Shape2D>> selectedShapes;
    for (const auto& token : m_scene->tokens)
    {
        if (token->isSelected)
            selectedShapes.push_back(static_cast<std::shared_ptr<Shape2D>>(token));
    }

    for (const auto& image : m_scene->images)
    {
        if (image->isSelected)
            selectedShapes.push_back(static_cast<std::shared_ptr<Shape2D>>(image));
    }

    return selectedShapes;
}

std::vector<std::shared_ptr<Token>> Controller::SelectedTokens()
{
    // TODO: Could this return an iterator? would be nicer.
    std::vector<std::shared_ptr<Token>> selectedTokens;
    for (const auto& token : m_scene->tokens)
    {
        if (token->isSelected)
            selectedTokens.push_back(token);
    }

    return selectedTokens;
}

std::vector<std::shared_ptr<BGImage>> Controller::SelectedImages()
{
    // TODO: Could this return an iterator? would be nicer.
    std::vector<std::shared_ptr<BGImage>> selectedImages;
    for (const auto& image : m_scene->images)
    {
        if (image->isSelected)
            selectedImages.push_back(image);
    }

    return selectedImages;
}

bool Controller::HasSelectedTokens()
{
    for (const auto& token : m_scene->tokens)
    {
        if (token->isSelected)
            return true;
    }
    return false;
}

bool Controller::HasSelectedImages()
{
    for (const auto& image : m_scene->images)
    {
        if (image->isSelected)
            return true;
    }
    return false;
}

bool Controller::HasSelectedShapes()
{
    return HasSelectedTokens() || HasSelectedImages();
}

void Controller::ClearSelection()
{
    PerformAction(std::make_shared<SelectShapesAction>(SelectedShapes()));
}

void Controller::SelectShape(std::shared_ptr<Shape2D> shape, bool additive)
{
    // Don't allow scene selection from UI selection if locked.
    auto token = std::dynamic_pointer_cast<Token>(shape);
    if (token)
    {
        m_uiWindow->SetDisplayPropertiesToken(token);
        if (m_scene->GetTokensLocked())
            return;
    }

    auto image = std::dynamic_pointer_cast<BGImage>(shape);
    if (image)
    {
        m_uiWindow->SetDisplayPropertiesImage(image);
        if (m_scene->GetImagesLocked())
            return;
    }

    PerformAction(std::make_shared<SelectShapesAction>(SelectedShapes(), shape, additive));
}

// void Controller::SelectShape(const std::shared_ptr<Shape2D>& shape, bool additive)
// {
//     PerformAction(std::make_shared<SelectShapesAction>(SelectedShapes(), shape, additive));
// }

void Controller::SelectShapes(const std::vector<std::shared_ptr<Shape2D>>& shapes, bool additive)
{
    PerformAction(std::make_shared<SelectShapesAction>(SelectedShapes(), shapes, additive));
    for (const auto& shape: shapes)
    {
        auto token = std::dynamic_pointer_cast<Token>(shape);
        if (token)
        {
            m_uiWindow->SetDisplayPropertiesToken(token);
        }
        auto image = std::dynamic_pointer_cast<BGImage>(shape);
        if (image)
        {
            m_uiWindow->SetDisplayPropertiesImage(image);
        }
    }
}

// Selection Operators
bool Controller::CopySelected()
{
    if (!HasSelectedShapes())
        return false;

    std::string string = m_serializer.SerializeScene(m_scene, SerializeFlag::Image |SerializeFlag::Token | SerializeFlag::Selected).dump();
    m_viewport->CopyToClipboard(string);
    return true;
}

void Controller::CutSelected()
{
    CopySelected();
    DeleteSelected();
}

void Controller::PasteSelected()
{
    std::string text = m_viewport->GetClipboard();
    if (text.empty())
        return;
    
    std::shared_ptr<Scene> scene = m_serializer.DeserializeScene(text);
    Merge(scene);
}

void Controller::DuplicateSelected()
{
    if (CopySelected())
        PasteSelected();
}

void Controller::DeleteSelected()
{
    std::shared_ptr<ActionGroup> actionGroup = std::make_shared<ActionGroup>();

    auto selectedTokens = SelectedTokens();
    if (!selectedTokens.empty())
        actionGroup->Add(std::make_shared<RemoveTokensAction>(m_scene, selectedTokens));

    auto selectedImages = SelectedImages();
    if (!selectedImages.empty())
        actionGroup->Add(std::make_shared<RemoveImagesAction>(m_scene, selectedImages));

    PerformAction(actionGroup);
}

// Viewport
void Controller::Focus()
{
    if (m_scene->IsEmpty())
        return;
    m_viewport->Focus(m_scene->GetBounds());
}

void Controller::FocusSelected()
{
    if (!HasSelectedShapes())
        return;

    m_viewport->Focus(Bounds2D::BoundsForShapes(SelectedShapes()));
}

void Controller::CloneCamera()
{
    auto camera = std::make_shared<Camera>(*m_viewport->GetCamera());
    camera->SetName(camera->GetName() + "Copy");

    std::shared_ptr<ActionGroup> actionGroup = std::make_shared<ActionGroup>();
    actionGroup->Add(std::make_shared<AddCameraAction>(m_scene, camera));
    actionGroup->Add(std::make_shared<SetViewCameraAction>(m_scene, PRIMARY, m_scene->GetViewCamera(PRIMARY), camera));
    PerformAction(actionGroup);
}

void Controller::DeleteCamera()
{
    if (m_scene->cameras.size() <= 1)
    {
        std::cerr << "Cannot delete last camera" << std::endl;
        return;
    }

    auto currentCamera = m_scene->GetViewCamera(PRIMARY);

    std::shared_ptr<ActionGroup> actionGroup = std::make_shared<ActionGroup>();
    actionGroup->Add(std::make_shared<RemoveCameraAction>(m_scene, currentCamera));
    for (const auto& camera: m_scene->cameras)
    {
        if (camera !=  currentCamera)
        {
            actionGroup->Add(std::make_shared<SetViewCameraAction>(m_scene, PRIMARY, currentCamera, camera));
            break;
        }
    }
    PerformAction(actionGroup);
}

void Controller::SetHostCamera(const std::shared_ptr<Camera>& camera)
{
    auto it = std::find(m_scene->cameras.begin(), m_scene->cameras.end(), camera);
    if (it == m_scene->cameras.end())
        return;

    PerformAction(std::make_shared<SetViewCameraAction>(m_scene, PRIMARY, m_scene->GetViewCamera(PRIMARY), camera));
}

// Screen Position
std::vector<std::shared_ptr<Shape2D>> Controller::ShapesInScreenRect(float minx, float miny, float maxx, float maxy)
{
    glm::vec2 lo = m_viewport->ScreenToWorldPos(minx, miny);
    glm::vec2 hi = m_viewport->ScreenToWorldPos(maxx, maxy);

    std::vector<std::shared_ptr<Shape2D>> shapes;

    if (!m_scene->GetTokensLocked())
    {
        for (const std::shared_ptr<Token>& token: m_scene->tokens)
        {
            float radius = token->GetModel()->GetScalef() * 0.5f;
            glm::vec2 tokenPos = token->GetModel()->GetPos();
            if (tokenPos.x + radius > lo.x && tokenPos.x - radius < hi.x
                && tokenPos.y + radius > lo.y && tokenPos.y - radius < hi.y)
            {
                shapes.push_back(static_cast<std::shared_ptr<Shape2D>>(token));
            }
        }
    }

    if (!m_scene->GetImagesLocked())
    {
        for (const std::shared_ptr<BGImage>& image: m_scene->images)
        {
            glm::vec2 scale = image->GetModel()->GetScale() * 0.5f;
            glm::vec2 imageMin = image->GetModel()->GetPos() - scale;
            glm::vec2 imageMax = image->GetModel()->GetPos() + scale;
            if (imageMax.x > lo.x && imageMin.x < hi.x && imageMax.y > lo.y && imageMin.y < hi.y)
            {
                shapes.push_back(static_cast<std::shared_ptr<Shape2D>>(image));
            }
        }
    }

    return shapes;
}

std::shared_ptr<Shape2D> Controller::GetShapeAtScreenPos(glm::vec2 screenPos)
{
    glm::vec2 worldPos = m_viewport->ScreenToWorldPos(screenPos.x, screenPos.y);
    // Tokens are drawn from first to last, so iterate in reverse to find the topmost
    if (!m_scene->GetTokensLocked())
    {
        for (int i = m_scene->tokens.size() - 1; i >= 0; i--)
        {
            // It should only be possible to select one shape with a single click
            if (m_scene->tokens[i]->Contains(worldPos))
            {
                return static_cast<std::shared_ptr<Shape2D>>(m_scene->tokens[i]);
            }
        }
    }
    // Images are drawn from first to last, so iterate in reverse to find the topmost
    if (!m_scene->GetImagesLocked())
    {
        for (int i = m_scene->images.size() - 1; i >= 0; i--)
        {
            // It should only be possible to select one shape with a single click
            if (m_scene->images[i]->Contains(worldPos))
            {
                return static_cast<std::shared_ptr<Shape2D>>(m_scene->images[i]);
            }
        }
    }
    return nullptr;
}

// Drag Selection
bool Controller::IsDragSelecting()
{
    return static_cast<bool>(dragSelectRect);
}

void Controller::StartDragSelection(float xpos, float ypos)
{
    dragSelectRect = std::make_shared<RectOverlay>(
        m_resources->GetMesh(Resources::MeshType::Quad2),
        m_resources->GetShader(Resources::ShaderType::ScreenRect),
        glm::vec4(SELECTION_COLOR, OVERLAY_OPACITY)
    );
    // GL uses inverted Y-axis
    dragSelectRect->startCorner = dragSelectRect->endCorner = glm::vec2(xpos, m_viewport->Height() - ypos);
    m_scene->overlays.push_back(static_cast<std::shared_ptr<Overlay>>(dragSelectRect));
}

void Controller::UpdateDragSelection(float xpos, float ypos)
{
    // GL uses inverted Y-axis
    dragSelectRect->endCorner = glm::vec2(xpos, m_viewport->Height() - ypos);

    // Y-axis is inverted on rect, use re-invert for calculating world positions
    auto coveredShapes = ShapesInScreenRect(
        dragSelectRect->MinX(),
        m_viewport->Height() - dragSelectRect->MinY(),
        dragSelectRect->MaxX(),
        m_viewport->Height() - dragSelectRect->MaxY()
    );

    for (const std::shared_ptr<Shape2D>& shape : coveredShapes)
        shape->isHighlighted = true;
}

void Controller::FinishDragSelection(bool additive)
{
    // Y-axis is inverted on rect, use re-invert for calculating world positions
    auto shapesInBounds = ShapesInScreenRect(
        dragSelectRect->MinX(),
        m_viewport->Height() - dragSelectRect->MinY(),
        dragSelectRect->MaxX(),
        m_viewport->Height() - dragSelectRect->MaxY()
    );

    if (shapesInBounds.size() > 0)
        SelectShapes(shapesInBounds, additive);
    else if (HasSelectedShapes())
        ClearSelection();

    m_scene->RemoveOverlay(static_cast<std::shared_ptr<Overlay>>(dragSelectRect));
    dragSelectRect.reset();
}

// Input Callbacks
void Controller::OnViewportMouseMove(double xpos, double ypos)
{
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseX = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // reversed since y-coordinates range from bottom to top
    lastMouseX = xpos;
    lastMouseY = ypos;

    glm::vec2 worldPos = m_viewport->ScreenToWorldPos(xpos, ypos);
    // TODO: Change this.
    //   Can be optimised to track what's highlighted and explicitly clear it.
    //   Also shouldn't rely on the `lock` options in mouse move
    // Only highlight the first matching shape, but unhighlight any others that were highlighted
    // Shapes are drawn from first to last, so iterate in reverse to find the topmost
    bool highlighted = false;
    if (!m_scene->GetTokensLocked())
    {
        for (int i = m_scene->tokens.size() - 1; i >= 0; i--)
        {
            std::shared_ptr<Token>& token = m_scene->tokens[i];
            token->isHighlighted = token->Contains(worldPos) && !highlighted;
            highlighted |= token->isHighlighted;
        }
    }
    if (!m_scene->GetImagesLocked())
    {
        for (int i = m_scene->images.size() - 1; i >= 0; i--)
        {
            std::shared_ptr<BGImage>& image = m_scene->images[i];
            image->isHighlighted = image->Contains(worldPos) && !highlighted;
            highlighted |= image->isHighlighted;
        }
    }

    if (middleMouseHeld)
    {
        m_viewport->GetCamera()->Pan(m_viewport->ScreenToWorldOffset(xoffset, yoffset));
        m_viewport->RefreshCamera();
    }
    else if (leftMouseHeld && shapeUnderCursor)
    {
        if (m_scene->grid->GetSnapEnabled())
        {
            glm::vec2 newPos = m_scene->grid->ShapeSnapPosition(shapeUnderCursor, m_viewport->ScreenToWorldPos(xpos, ypos));
            glm::vec2 currPos = shapeUnderCursor->GetModel()->GetPos();
            if (newPos != currPos)
            {
                std::shared_ptr<ActionGroup> actionGroup = std::make_shared<ActionGroup>();
                glm::vec2 offset = newPos - currPos;
                for (const std::shared_ptr<Shape2D>& shape : SelectedShapes())
                    actionGroup->Add(std::make_shared<ModifyMatrix2DVec2>(shape->GetModel(), &Matrix2D::SetPos, shape->GetModel()->GetPos(), shape->GetModel()->GetPos() + offset));

                PerformAction(actionGroup);
            }
        }
        else
        {
            std::shared_ptr<ActionGroup> actionGroup = std::make_shared<ActionGroup>();
            glm::vec2 offset = m_viewport->ScreenToWorldOffset(xoffset, yoffset);
            for (std::shared_ptr<Shape2D> shape : SelectedShapes())
                actionGroup->Add(std::make_shared<ModifyMatrix2DVec2>(shape->GetModel(), &Matrix2D::SetPos, shape->GetModel()->GetPos(), shape->GetModel()->GetPos() + offset));

            PerformAction(actionGroup);
        }
    }
    else if (leftMouseHeld && IsDragSelecting())
        UpdateDragSelection(xpos, ypos);
}

void Controller::OnViewportMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        middleMouseHeld = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            // TODO: Selection action should be combined with move action, ie, undo undoes the selection and the movement
            glm::vec2 cursorPos = m_viewport->CursorPos();
            shapeUnderCursor = GetShapeAtScreenPos(cursorPos);
            if (shapeUnderCursor && !shapeUnderCursor->isSelected)
                SelectShape(shapeUnderCursor, mods & (GLFW_MOD_SHIFT | GLFW_MOD_CONTROL));
            // If nothing was immediately selected/being modified, start a drag select
            else if (!shapeUnderCursor)
                StartDragSelection(cursorPos.x, cursorPos.y);
        }
        else if (action == GLFW_RELEASE)
        {
            shapeUnderCursor = nullptr;
            if (IsDragSelecting())
                FinishDragSelection(mods & GLFW_MOD_SHIFT);
        }
        leftMouseHeld = action == GLFW_PRESS;
    }
}

void Controller::OnViewportMouseScroll(double xoffset, double yoffset)
{
    m_viewport->GetCamera()->Zoom(yoffset);
    m_viewport->RefreshCamera();
}

void Controller::OnViewportKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        m_viewport->SetFullscreen(!m_viewport->IsFullscreen());
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE && HasSelectedShapes())
    {
        for (std::shared_ptr<Shape2D> shape : SelectedShapes())
        {
            ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(shape) + 1);
            shape->GetModel()->SetScalef(m_scene->grid->SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE && HasSelectedShapes())
    {
        for (std::shared_ptr<Shape2D> shape : SelectedShapes())
        {
            ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(shape) - 1);
            shape->GetModel()->SetScalef(m_scene->grid->SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_DELETE && HasSelectedShapes())
        DeleteSelected();
    if (key == GLFW_KEY_D && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL)
        DuplicateSelected();
    if (key == GLFW_KEY_C && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL)
        CopySelected();
    if (key == GLFW_KEY_X && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL)
        CutSelected();
    if (key == GLFW_KEY_V && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL)
        PasteSelected();
    OnKeyChanged(key, scancode, action, mods);
}

void Controller::OnViewportSizeChanged(int width, int height)
{
    m_viewport->RefreshCamera();
}

void Controller::OnUIAddTokenClicked()
{
    auto token = std::make_shared<Token>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(Resources::TextureType::Default)
    );
    // Centers it on the camera view
    token->GetModel()->SetPos(glm::vec2(m_viewport->GetCamera()->Position.x, m_viewport->GetCamera()->Position.y));
    std::shared_ptr<AddTokensAction> action = std::make_shared<AddTokensAction>(m_scene, token);
    // TODO: Include selection
    PerformAction(action);
}

void Controller::OnUIAddImageClicked()
{
    PerformAction(std::make_shared<AddImagesAction>(m_scene, std::make_shared<BGImage>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(Resources::TextureType::Default)
    )));
}

void Controller::OnUIRemoveImageClicked(const std::shared_ptr<BGImage>& image)
{
    PerformAction(std::make_shared<RemoveImagesAction>(m_scene, image));
}

void Controller::OnUIKeyChanged(int key, int scancode, int action, int mods)
{
    OnKeyChanged(key, scancode, action, mods);
}

void Controller::PerformAction(const std::shared_ptr<Action>& action)
{
    action->Redo();
    if (undoQueue.size() > 0 && undoQueue.back()->CanMerge(action))
        undoQueue.back()->Merge(action);
    else
    {
        undoQueue.push_back(std::move(action));
        if (undoQueue.size() > MAX_UNDO_SIZE)
            undoQueue.pop_front();
        redoQueue.clear();
    }
}

void Controller::OnTokenPropertyChanged(const std::shared_ptr<Token>& token, TokenProperty property, TokenPropertyValue value)
{
    auto actionGroup = std::make_shared<ActionGroup>();
    switch (property)
    {
    case Token_Name:
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyTokenString>(selectedToken, &Token::SetName, selectedToken->GetName(), std::get<std::string>(value)));
        break;
    case Token_Position:
    {
        glm::vec2 pos = std::get<glm::vec2>(value);
        if (m_scene->grid->GetSnapEnabled())
            pos = m_scene->grid->ShapeSnapPosition(token, pos);
        glm::vec2 offset = pos - token->GetModel()->GetPos();
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyMatrix2DVec2>(selectedToken->GetModel(), &Matrix2D::SetPos, selectedToken->GetModel()->GetPos(), selectedToken->GetModel()->GetPos() + offset));
        break;
    }
    case Token_Rotation:
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyMatrix2DFloat>(selectedToken->GetModel(), &Matrix2D::SetRotation, selectedToken->GetModel()->GetRotation(), std::get<float>(value)));
        break;
    case Token_Scale:
        for (const auto& selectedToken: SelectedTokens())
        {
            glm::vec2 scale = std::get<glm::vec2>(value);
            if (m_scene->grid->GetSnapEnabled())
            {
                ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(scale.x));
                scale = glm::vec2(m_scene->grid->SnapGridSize(gridSize));
            }
            actionGroup->Add(std::make_shared<ModifyMatrix2DVec2>(selectedToken->GetModel(), &Matrix2D::SetScale, selectedToken->GetModel()->GetScale(), scale));
        }
        break;
    case Token_BorderWidth:
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyTokenFloat>(selectedToken, &Token::SetBorderWidth, selectedToken->GetBorderWidth(), std::get<float>(value)));
        break;
    case Token_BorderColor:
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyTokenVec4>(selectedToken, &Token::SetBorderColor, selectedToken->GetBorderColor(), std::get<glm::vec4>(value)));
        break;
    case Token_Texture:
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyTokenTexture>(selectedToken, &Token::SetIcon, selectedToken->GetIcon(), m_resources->GetTexture(std::get<std::string>(value))));
        break;
    case Token_Statuses:
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyTokenStatuses>(selectedToken, &Token::SetStatuses, selectedToken->GetStatuses(), std::get<TokenStatuses>(value)));
        break;
    case Token_XStatus:
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyTokenBool>(selectedToken, &Token::SetXStatus, selectedToken->GetXStatus(), std::get<bool>(value)));
        break;
    case Token_Opacity:
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyTokenFloat>(selectedToken, &Token::SetOpacity, selectedToken->GetOpacity(), std::get<float>(value)));
        break;
    
    default:
        std::cerr << "Unknown TokenProperty: " << property << std::endl;
        actionGroup.reset();
        break;
    }
    
    if (!actionGroup->IsEmpty())
        PerformAction(actionGroup);
}

void Controller::OnImagePropertyChanged(const std::shared_ptr<BGImage>& image, ImageProperty property, ImagePropertyValue value)
{
    std::shared_ptr<Action> action;
    switch (property)
    {
    case Image_Texture:
        action = std::make_shared<ModifyImageTexture>(image, &BGImage::SetImage, image->GetImage(), m_resources->GetTexture(std::get<std::string>(value)));
        break;
    case Image_LockRatio:
        action = std::make_shared<ModifyImageBool>(image, &BGImage::SetLockRatio, image->GetLockRatio(), std::get<bool>(value));
        break;
    case Image_Position:
        action = std::make_shared<ModifyMatrix2DVec2>(image->GetModel(), &Matrix2D::SetPos, image->GetModel()->GetPos(), std::get<glm::vec2>(value));
        break;
    case Image_Rotation:
        action = std::make_shared<ModifyMatrix2DFloat>(image->GetModel(), &Matrix2D::SetRotation, image->GetModel()->GetRotation(), std::get<float>(value));
        break;
    case Image_Scale:
        action = std::make_shared<ModifyMatrix2DVec2>(image->GetModel(), &Matrix2D::SetScale, image->GetModel()->GetScale(), std::get<glm::vec2>(value));
        break;
    
    default:
        std::cerr << "Unknown ImageProperty: " << property << std::endl;
        break;
    }
    
    if (action)
        PerformAction(action);
}

void Controller::OnGridPropertyChanged(const std::shared_ptr<Grid>& grid, GridProperty property, GridPropertyValue value)
{
    std::shared_ptr<Action> action;
    switch (property)
    {
    case Grid_Scale:
        action = std::make_shared<ModifyGridFloat>(grid, &Grid::SetScale, grid->GetScale(), std::get<float>(value));
        break;
    case Grid_Snap:
        action = std::make_shared<ModifyGridBool>(grid, &Grid::SetSnapEnabled, grid->GetSnapEnabled(), std::get<bool>(value));
        break;
    case Grid_Color:
        action = std::make_shared<ModifyGridVec3>(grid, &Grid::SetColour, grid->GetColour(), std::get<glm::vec3>(value));
        break;
    
    default:
        std::cerr << "Unknown GridProperty: " << property << std::endl;
        break;
    }
    
    if (action)
        PerformAction(action);
}

void Controller::OnCameraPropertyChanged(const std::shared_ptr<Camera>& camera, CameraProperty property, CameraPropertyValue value)
{
    std::shared_ptr<Action> action;
    switch (property)
    {
    case Camera_Name:
        action = std::make_shared<ModifyCameraString>(camera, &Camera::SetName, camera->GetName(), std::get<std::string>(value));
        break;
    
    default:
        std::cerr << "Unknown CameraProperty: " << property << std::endl;
        break;
    }

    if (action)
        PerformAction(action);
}

bool Controller::Undo()
{
    if (undoQueue.size() == 0)
        return false;
    
    auto action = undoQueue.back();
    action->Undo();
    redoQueue.push_back(action);
    undoQueue.pop_back();
    return true;
}

bool Controller::Redo()
{
    if (redoQueue.size() == 0)
        return false;
    
    auto action = redoQueue.back();
    action->Redo();
    undoQueue.push_back(action);
    redoQueue.pop_back();
    return true;
}

void Controller::OnPromptResponse(int promptType, bool response)
{
    switch (promptType)
    {
    case PROMPT_CLOSE:
        if (response)
            m_viewport->Close();
        break;
    
    default:
        std::cerr << "Unknown prompt response: " << promptType << " got " << response << std::endl;
        break;
    }
}

void Controller::OnCloseRequested()
{
    m_uiWindow->Prompt(PROMPT_CLOSE, "Are you sure you want to quit?");
}

void Controller::OnKeyChanged(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        m_scene->grid->SetSnapEnabled(!m_scene->grid->GetSnapEnabled());
    if (key == GLFW_KEY_S && mods & GLFW_MOD_CONTROL && action == GLFW_RELEASE && !m_scene->sourceFile.empty())
        Save(m_scene->sourceFile);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        OnCloseRequested();
    if (key == GLFW_KEY_Z && action == GLFW_RELEASE && mods & GLFW_MOD_CONTROL)
        Undo();
    if (key == GLFW_KEY_Y && action == GLFW_RELEASE && mods & GLFW_MOD_CONTROL)
        Redo();
    if (key == GLFW_KEY_F && action == GLFW_RELEASE)
        HasSelectedShapes() ? FocusSelected() : Focus();
    if (key == GLFW_KEY_X && action == GLFW_RELEASE && HasSelectedShapes())
    {
        auto actionGroup = std::make_shared<ActionGroup>();
        for (const auto& selectedToken: SelectedTokens())
            actionGroup->Add(std::make_shared<ModifyTokenBool>(selectedToken, &Token::SetXStatus, selectedToken->GetXStatus(), !selectedToken->GetXStatus()));
        PerformAction(actionGroup);
    }
}

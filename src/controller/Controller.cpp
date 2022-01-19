#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include <JSONSerializer.h>
#include <Resources.h>
#include <model/Overlays.h>
#include <model/Scene.h>
#include <model/Token.h>
#include <view/UIState.h>
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
    m_uiWindow->tokenSelectionChanged.connect(this, &Controller::SelectToken);
    m_uiWindow->closeRequested.connect(this, &Controller::OnCloseRequested);
    m_uiWindow->keyChanged.connect(this, &Controller::OnUIKeyChanged);
    // TODO: Fails because the Signal method can't capture unique_ptr by value in the lambda
    m_uiWindow->actionTaken.connect(this, &Controller::PerformAction);

    m_uiWindow->uiState = uiState;
    SetScene(std::make_shared<Scene>(m_resources));
}

// Scene Management
void Controller::SetScene(std::shared_ptr<Scene> scene)
{
    m_uiWindow->addImageClicked.disconnect();
    m_uiWindow->addTokenClicked.disconnect();

    m_scene = scene;
    m_uiWindow->addImageClicked.connect(m_scene.get(), &Scene::AddImage);
    m_uiWindow->addTokenClicked.connect(m_scene.get(), &Scene::AddToken);

    m_viewport->SetScene(scene);
    m_uiWindow->SetScene(scene);
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
        if (!merge)
        {
            m_scene->images.clear();
            m_scene->tokens.clear();
        }
        m_serializer.DeserializeScene(j, *m_scene);
        m_scene->sourceFile = path;
        SetScene(m_scene);
    }
    else
        std::cerr << "Unable to open file" << std::endl;
}


// Selection
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

bool Controller::HasSelectedTokens()
{
    for (const auto& token : m_scene->tokens)
    {
        if (token->isSelected)
            return true;
    }
    return false;
}

void Controller::ClearSelection()
{
    for (std::shared_ptr<Token>& token : m_scene->tokens)
        token->isSelected = false;
}

void Controller::SelectToken(std::shared_ptr<Token> token, bool additive)
{
    if (!additive)
        ClearSelection();
    token->isSelected = true;
}

void Controller::DuplicateSelectedTokens()
{
    uint numTokens = m_scene->tokens.size();
    for (std::shared_ptr<Token> token : SelectedTokens())
    {
        std::shared_ptr<Token> duplicate = std::make_shared<Token>(*token);
        duplicate->GetModel()->Offset(glm::vec2(1));
        m_scene->AddToken(duplicate);
    }

    ClearSelection();
    for (uint i = numTokens; i < m_scene->tokens.size(); i++)
        SelectToken(m_scene->tokens[i]);
}

// Screen Position
std::vector<std::shared_ptr<Token>> Controller::TokensInScreenRect(float minx, float miny, float maxx, float maxy)
{
    glm::vec2 lo = m_viewport->ScreenToWorldPos(minx, miny);
    glm::vec2 hi = m_viewport->ScreenToWorldPos(maxx, maxy);

    std::vector<std::shared_ptr<Token>> tokens;
    for (std::shared_ptr<Token> token: m_scene->tokens)
    {
        float radius = token->GetModel()->GetScalef() * 0.5f;
        glm::vec2 tokenPos = token->GetModel()->GetPos();
        if (tokenPos.x + radius > lo.x && tokenPos.x - radius < hi.x
            && tokenPos.y + radius > lo.y && tokenPos.y - radius < hi.y)
        {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::shared_ptr<Token> Controller::GetTokenAtScreenPos(glm::vec2 screenPos)
{
    glm::vec2 worldPos = m_viewport->ScreenToWorldPos(screenPos.x, screenPos.y);
    // Tokens are drawn from first to last, so iterate in reverse to find the topmost
    for (int i = m_scene->tokens.size() - 1; i >= 0; i--)
    {
        std::shared_ptr<Token> token = m_scene->tokens[i];
        // It should only be possible to select one token with a single click
        if (token->Contains(worldPos))
        {
            return token;
        }
    }
    return nullptr;
}

// Drag Selection
bool Controller::IsDragSelecting()
{
    return static_cast<bool>(uiState->dragSelectRect);
}

void Controller::StartDragSelection(float xpos, float ypos)
{
    uiState->dragSelectRect = std::make_shared<RectOverlay>(
        m_resources->GetMesh(Resources::MeshType::Quad2),
        m_resources->GetShader(Resources::ShaderType::ScreenRect)
    );
    // GL uses inverted Y-axis
    uiState->dragSelectRect->startCorner = uiState->dragSelectRect->endCorner = glm::vec2(xpos, m_viewport->Height() - ypos);
    m_scene->overlays.push_back(static_cast<std::shared_ptr<Overlay>>(uiState->dragSelectRect));
}

void Controller::UpdateDragSelection(float xpos, float ypos)
{
    // GL uses inverted Y-axis
    uiState->dragSelectRect->endCorner = glm::vec2(xpos, m_viewport->Height() - ypos);

    // Y-axis is inverted on rect, use re-invert for calculating world positions
    auto coveredTokens = TokensInScreenRect(
        uiState->dragSelectRect->MinX(),
        m_viewport->Height() - uiState->dragSelectRect->MinY(),
        uiState->dragSelectRect->MaxX(),
        m_viewport->Height() - uiState->dragSelectRect->MaxY()
    );

    for (std::shared_ptr<Token> token : coveredTokens)
        token->isHighlighted = true;
}

void Controller::FinishDragSelection()
{
    // Y-axis is inverted on rect, use re-invert for calculating world positions
    auto tokensInBounds = TokensInScreenRect(
        uiState->dragSelectRect->MinX(),
        m_viewport->Height() - uiState->dragSelectRect->MinY(),
        uiState->dragSelectRect->MaxX(),
        m_viewport->Height() - uiState->dragSelectRect->MaxY()
    );

    for (std::shared_ptr<Token> token : tokensInBounds)
        SelectToken(token);

    m_scene->RemoveOverlay(static_cast<std::shared_ptr<Overlay>>(uiState->dragSelectRect));
    uiState->dragSelectRect.reset();
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
    bool highlighted = false;
    for (int i = m_scene->tokens.size() - 1; i >= 0; i--)
    {
        // Only highlight the first matching token, but unhighlight any others that were highlighted
        // Tokens are drawn from first to last, so iterate in reverse to find the topmost
        std::shared_ptr<Token> token = m_scene->tokens[i];
        token->isHighlighted = token->Contains(worldPos) && !highlighted;
        highlighted |= token->isHighlighted;
    }

    if (middleMouseHeld)
    {
        m_scene->camera->Pan(m_viewport->ScreenToWorldOffset(xoffset, yoffset));
        m_viewport->RefreshCamera();
    }
    else if (leftMouseHeld && uiState->tokenUnderCursor)
    {
        if (uiState->snapToGrid)
        {
            glm::vec2 newPos = m_scene->grid->ShapeSnapPosition(uiState->tokenUnderCursor, m_viewport->ScreenToWorldPos(xpos, ypos));
            glm::vec2 currPos = uiState->tokenUnderCursor->GetModel()->GetPos();
            if (newPos != currPos)
            {
                glm::vec2 offset = newPos - currPos;
                for (std::shared_ptr<Token> token : SelectedTokens())
                    token->GetModel()->Offset(offset);
            }
        }
        else
        {
            glm::vec2 offset = m_viewport->ScreenToWorldOffset(xoffset, yoffset);
            for (std::shared_ptr<Token> token : SelectedTokens())
                token->GetModel()->Offset(offset);
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
            glm::vec2 cursorPos = m_viewport->CursorPos();
            uiState->tokenUnderCursor = GetTokenAtScreenPos(cursorPos);
            if (uiState->tokenUnderCursor && !uiState->tokenUnderCursor->isSelected)
                SelectToken(uiState->tokenUnderCursor, mods & GLFW_MOD_SHIFT);
            // If nothing was immediately selected/being modified, start a drag select
            else if (!uiState->tokenUnderCursor)
            {
                if (!mods & GLFW_MOD_SHIFT)
                    ClearSelection();

                StartDragSelection(cursorPos.x, cursorPos.y);
            }
        }
        else if (action == GLFW_RELEASE)
        {
            uiState->tokenUnderCursor = nullptr;
            if (IsDragSelecting())
                FinishDragSelection();
        }
        leftMouseHeld = action == GLFW_PRESS;
    }
}

void Controller::OnViewportMouseScroll(double xoffset, double yoffset)
{
    m_scene->camera->Zoom(yoffset);
    m_viewport->RefreshCamera();
}

void Controller::OnViewportKey(int key, int scancode, int action, int mods)
{
    // TODO: Confirmation dialog on escape
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        OnCloseRequested();
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        uiState->snapToGrid = !uiState->snapToGrid;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        m_viewport->SetFullscreen(!m_viewport->IsFullscreen());
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE && HasSelectedTokens())
    {
        for (std::shared_ptr<Token> token : SelectedTokens())
        {
            ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(token) + 1);
            token->GetModel()->SetScalef(m_scene->grid->SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE && HasSelectedTokens())
    {
        for (std::shared_ptr<Token> token : SelectedTokens())
        {
            ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(token) - 1);
            token->GetModel()->SetScalef(m_scene->grid->SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_DELETE && HasSelectedTokens())
    {
        m_scene->RemoveTokens(SelectedTokens());
        ClearSelection();
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL && HasSelectedTokens())
        DuplicateSelectedTokens();
    if (key == GLFW_KEY_Z && action == GLFW_RELEASE && mods & GLFW_MOD_CONTROL)
        Undo();
    if (key == GLFW_KEY_Y && action == GLFW_RELEASE && mods & GLFW_MOD_CONTROL)
        Redo();
}

void Controller::OnViewportSizeChanged(int width, int height)
{
    m_viewport->RefreshCamera();
}

void Controller::OnUIAddTokenClicked()
{
    m_scene->AddToken();
    SelectToken(m_scene->tokens.back(), false);
}

void Controller::OnUIKeyChanged(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        OnCloseRequested();
    if (key == GLFW_KEY_Z && action == GLFW_RELEASE && mods & GLFW_MOD_CONTROL)
        Undo();
    if (key == GLFW_KEY_Y && action == GLFW_RELEASE && mods & GLFW_MOD_CONTROL)
        Redo();
}

void Controller::PerformAction(std::shared_ptr<Action> action)
{
    action->Redo();
    undoQueue.push_back(std::move(action));
    if (undoQueue.size() > MAX_UNDO_SIZE)
        undoQueue.pop_front();
    std::cout << "Saved actions: " << undoQueue.size() << std::endl;
    redoQueue.clear();
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
    redoQueue.push_back(action);
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

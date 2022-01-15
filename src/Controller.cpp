#include <memory>
#include <vector>

#include <Overlays.h>
#include <Resources.h>
#include <Scene.h>
#include <Token.h>
#include <UIState.h>
#include <UIWindow.h>
#include <Viewport.h>

#include <Controller.h>


Controller::Controller(std::shared_ptr<Resources> resources, std::shared_ptr<Scene> scene, std::shared_ptr<Viewport> viewport, std::shared_ptr<UIWindow> uiWindow) :
    m_resources(resources), m_scene(scene), m_viewport(viewport), m_uiWindow(uiWindow)
{
    m_viewport->cursorMoved.connect(this, &Controller::OnViewportMouseMove);
    m_viewport->keyChanged.connect(this, &Controller::OnViewportKey);
    m_viewport->mouseButtonChanged.connect(this, &Controller::OnViewportMouseButton);
    m_viewport->mouseScrolled.connect(this, &Controller::OnViewportMouseScroll);
    m_viewport->sizeChanged.connect(this, &Controller::OnViewportSizeChanged);

    m_viewport->SetScene(scene);
    m_uiWindow->SetScene(scene);
}

// Selection
void Controller::ClearSelection()
{
    for (std::shared_ptr<Token>& token : uiState->selectedTokens)
        token->isSelected = false;
    uiState->selectedTokens.clear();
}

void Controller::SelectToken(std::shared_ptr<Token> token)
{
    uiState->selectedTokens.push_back(token);
    token->isSelected = true;
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
        m_scene->camera->Pan(m_viewport->ScreenToWorldOffset(xoffset, yoffset));
    else if (leftMouseHeld && uiState->tokenUnderCursor)
    {
        if (uiState->snapToGrid)
        {
            glm::vec2 newPos = m_scene->grid->ShapeSnapPosition(uiState->tokenUnderCursor, m_viewport->ScreenToWorldPos(xpos, ypos));
            glm::vec2 currPos = uiState->tokenUnderCursor->GetModel()->GetPos();
            if (newPos != currPos)
            {
                glm::vec2 offset = newPos - currPos;
                for (std::shared_ptr<Token> token : uiState->selectedTokens)
                    token->GetModel()->Offset(offset);
            }
        }
        else
        {
            glm::vec2 offset = m_viewport->ScreenToWorldOffset(xoffset, yoffset);
            for (std::shared_ptr<Token> token : uiState->selectedTokens)
                token->GetModel()->Offset(offset);
        }
    }
    else if (leftMouseHeld && uiState->dragSelectRect)
    {
        // GL uses inverted Y-axis
        uiState->dragSelectRect->endCorner = glm::vec2(xpos, m_viewport->Height() - ypos);

        // Y-axis is inverted on rect, use re-invert for calculating world positions
        auto selectedTokens = TokensInScreenRect(
            uiState->dragSelectRect->MinX(),
            m_viewport->Height() - uiState->dragSelectRect->MinY(),
            uiState->dragSelectRect->MaxX(),
            m_viewport->Height() - uiState->dragSelectRect->MaxY()
        );

        for (std::shared_ptr<Token> token : uiState->selectedTokens)
            token->isHighlighted = true;
    }
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
            {
                if (!mods & GLFW_MOD_SHIFT)
                    ClearSelection();
                SelectToken(uiState->tokenUnderCursor);
            }
            // If nothing was immediately selected/being modified, start a drag select
            else if (!uiState->tokenUnderCursor)
            {
                if (!mods & GLFW_MOD_SHIFT)
                    ClearSelection();
                uiState->dragSelectRect = std::make_unique<RectOverlay>(
                    m_resources->GetMesh(Resources::MeshType::Quad2),
                    m_resources->GetShader(Resources::ShaderType::ScreenRect)
                );
                // GL uses inverted Y-axis
                uiState->dragSelectRect->startCorner = uiState->dragSelectRect->endCorner = glm::vec2(cursorPos.x, m_viewport->Height() - cursorPos.y);
            }
        }
        else if (action == GLFW_RELEASE)
        {
            uiState->tokenUnderCursor = nullptr;
            if (uiState->dragSelectRect)
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

                uiState->dragSelectRect.reset();
            }
        }
        leftMouseHeld = action == GLFW_PRESS;
    }
}

void Controller::OnViewportMouseScroll(double xoffset, double yoffset)
{
    m_scene->camera->Zoom(yoffset);
}

void Controller::OnViewportKey(int key, int scancode, int action, int mods)
{
    // TODO: Confirmation dialog on escape
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        m_viewport->Close();
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        uiState->snapToGrid = !uiState->snapToGrid;
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE && uiState->selectedTokens.size() > 0)
    {
        for (std::shared_ptr<Token> token : uiState->selectedTokens)
        {
            ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(token) + 1);
            token->GetModel()->SetScalef(m_scene->grid->SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE && uiState->selectedTokens.size() > 0)
    {
        for (std::shared_ptr<Token> token : uiState->selectedTokens)
        {
            ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(token) - 1);
            token->GetModel()->SetScalef(m_scene->grid->SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_DELETE && uiState->selectedTokens.size() > 0)
    {
        m_scene->RemoveTokens(uiState->selectedTokens);
        ClearSelection();
    }
    // TODO: Scene->DuplicateToken()
    // if (key == GLFW_KEY_D && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL && m_uiState->selectedTokens.size() > 0)
    // {
    //     uint numTokens = m_scene->tokens.size();
    //     for (Token* token : m_uiState->selectedTokens)
    //     {
    //         // Copy the matrix with an offset
    //         Matrix2D matrix = *token->GetModel();
    //         matrix.Offset(glm::vec2(1));
    //         m_scene->AddToken(token->GetIcon(), matrix);
    //     }

    //     uiState.ClearSelection();
    //     for (uint i = numTokens; i < m_scene->tokens.size(); i++)
    //         uiState.SelectToken(&m_scene->tokens[i]);
    // }
}

void Controller::OnViewportSizeChanged(int width, int height)
{
    m_viewport->RefreshCamera();
}

void Controller::OnUIAddTokenClicked()
{
    ClearSelection();
    m_scene->AddToken();
    SelectToken(m_scene->tokens.back());
}

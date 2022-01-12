#pragma once
#include <memory>
#include <vector>

#include <Overlays.h>
#include <Resources.h>
#include <Scene.h>
#include <Token.h>
#include <Window.h>


class InputManager
{
public:
    InputManager(std::shared_ptr<Scene> scene, std::shared_ptr<Window> viewport, std::shared_ptr<Window> uiWindow);

    void ClearSelection();
    void SelectToken(std::shared_ptr<Token> token);

    void ResizeViewport(unsigned int width, unsigned int height);

    // Orthographic operations (Matrices aren't working correctly if camera position is changed)
    glm::vec2 ScreenToWorldPos(float x, float y);
    glm::vec2 ScreenToWorldOffset(float x, float y);
    std::vector<std::shared_ptr<Token>> TokensInScreenRect(float minx, float miny, float maxx, float maxy);
    std::shared_ptr<Token> GetTokenAtScreenPos(glm::vec2 screenPos);

    void SetCallbacks();
    void OnMouseMove(double xpos, double ypos);
    void OnMouseButton(int button, int action, int mods);
    void OnMouseScroll(double xoffset, double yoffset);
    void OnKey(int key, int scancode, int action, int mods);

private:
    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<Window> m_viewport;
    std::shared_ptr<Window> m_uiWindow;

    bool snapToGrid = false;
    std::shared_ptr<Resources> m_resources;
    std::vector<std::shared_ptr<Token>> m_selectedTokens;
    std::unique_ptr<RectOverlay> dragSelectRect;
    std::shared_ptr<Token> tokenUnderCursor;

    bool firstMouse = true;
    float lastMouseX, lastMouseY;
    bool middleMouseHeld = false;
    bool leftMouseHeld = false;
    float deltaTime, lastFrame = 0.0f;  // TODO: Should live in the app
};

InputManager::InputManager(std::shared_ptr<Scene> scene, std::shared_ptr<Window> viewport, std::shared_ptr<Window> uiWindow) :
    m_scene(scene), m_viewport(viewport), m_uiWindow(uiWindow)
{
    glfwSetWindowUserPointer(viewport->ID(), this);
}

// Selection
void InputManager::ClearSelection()
{
    for (std::shared_ptr<Token>& token : m_selectedTokens)
        token->isSelected = false;
    m_selectedTokens.clear();
}

void InputManager::SelectToken(std::shared_ptr<Token> token)
{
    m_selectedTokens.push_back(token);
    token->isSelected = true;
}

// Screen Position
std::vector<std::shared_ptr<Token>> InputManager::TokensInScreenRect(float minx, float miny, float maxx, float maxy)
{
    glm::vec2 lo = ScreenToWorldPos(minx, miny);
    glm::vec2 hi = ScreenToWorldPos(maxx, maxy);

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

std::shared_ptr<Token> InputManager::GetTokenAtScreenPos(glm::vec2 screenPos)
{
    glm::vec2 worldPos = ScreenToWorldPos(screenPos.x, screenPos.y);
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
void InputManager::SetCallbacks()
{
    glfwSetFramebufferSizeCallback(m_viewport->ID(), framebuffer_size_callback);
    glfwSetCursorPosCallback(m_viewport->ID(), mouse_callback);
    glfwSetMouseButtonCallback(m_viewport->ID(), mouse_button_callback);
    glfwSetScrollCallback(m_viewport->ID(), scroll_callback);
    glfwSetKeyCallback(m_viewport->ID(), key_callback);
}

void InputManager::OnMouseMove(double xpos, double ypos)
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

    glm::vec2 worldPos = ScreenToWorldPos(xpos, ypos);
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
        m_scene->camera->Pan(ScreenToWorldOffset(xoffset, yoffset));
    else if (leftMouseHeld && tokenUnderCursor)
    {
        if (snapToGrid)
        {
            glm::vec2 newPos = m_scene->grid->ShapeSnapPosition(tokenUnderCursor, ScreenToWorldPos(xpos, ypos));
            glm::vec2 currPos = tokenUnderCursor->GetModel()->GetPos();
            if (newPos != currPos)
            {
                glm::vec2 offset = newPos - currPos;
                for (std::shared_ptr<Token> token : m_selectedTokens)
                    token->GetModel()->Offset(offset);
            }
        }
        else
        {
            glm::vec2 offset = ScreenToWorldOffset(xoffset, yoffset);
            for (std::shared_ptr<Token> token : m_selectedTokens)
                token->GetModel()->Offset(offset);
        }
    }
    else if (leftMouseHeld && dragSelectRect)
    {
        // GL uses inverted Y-axis
        dragSelectRect->endCorner = glm::vec2(xpos, m_viewport->Height() - ypos);

        // Y-axis is inverted on rect, use re-invert for calculating world positions
        auto selectedTokens = TokensInScreenRect(
            dragSelectRect->MinX(),
            m_viewport->Height() - dragSelectRect->MinY(),
            dragSelectRect->MaxX(),
            m_viewport->Height() - dragSelectRect->MaxY()
        );

        for (std::shared_ptr<Token> token : m_selectedTokens)
            token->isHighlighted = true;
    }
}

void InputManager::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        middleMouseHeld = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            glm::vec2 cursorPos = m_viewport->CursorPos();
            tokenUnderCursor = GetTokenAtScreenPos(cursorPos);
            if (tokenUnderCursor && !tokenUnderCursor->isSelected)
            {
                if (!mods & GLFW_MOD_SHIFT)
                    ClearSelection();
                SelectToken(tokenUnderCursor);
            }
            // If nothing was immediately selected/being modified, start a drag select
            else if (!tokenUnderCursor)
            {
                if (!mods & GLFW_MOD_SHIFT)
                    ClearSelection();
                dragSelectRect = std::make_unique<RectOverlay>();
                // GL uses inverted Y-axis
                dragSelectRect->startCorner = dragSelectRect->endCorner = glm::vec2(cursorPos.x, m_viewport->Height() - cursorPos.y);
            }
        }
        else if (action == GLFW_RELEASE)
        {
            tokenUnderCursor = nullptr;
            if (dragSelectRect)
            {
                // Y-axis is inverted on rect, use re-invert for calculating world positions
                auto tokensInBounds = TokensInScreenRect(
                    dragSelectRect->MinX(),
                    m_viewport->Height() - dragSelectRect->MinY(),
                    dragSelectRect->MaxX(),
                    m_viewport->Height() - dragSelectRect->MaxY()
                );

                for (std::shared_ptr<Token> token : tokensInBounds)
                    SelectToken(token);

                dragSelectRect.reset();
            }
        }
        leftMouseHeld = action == GLFW_PRESS;
    }
}

void InputManager::OnMouseScroll(double xoffset, double yoffset)
{
    m_scene->camera->Zoom(yoffset);
}

void InputManager::OnKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        m_viewport->Close();
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        snapToGrid = !snapToGrid;
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE && m_selectedTokens.size() > 0)
    {
        for (std::shared_ptr<Token> token : m_selectedTokens)
        {
            ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(token) + 1);
            token->GetModel()->SetScalef(m_scene->grid->SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE && m_selectedTokens.size() > 0)
    {
        for (std::shared_ptr<Token> token : m_selectedTokens)
        {
            ShapeGridSize gridSize = static_cast<ShapeGridSize>(m_scene->grid->GetShapeGridSize(token) - 1);
            token->GetModel()->SetScalef(m_scene->grid->SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_DELETE && m_selectedTokens.size() > 0)
    {
        m_scene->RemoveTokens(m_selectedTokens);
        ClearSelection();
    }
    // if (key == GLFW_KEY_D && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL && m_selectedTokens.size() > 0)
    // {
    //     uint numTokens = m_scene->tokens.size();
    //     for (Token* token : m_selectedTokens)
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

void InputManager::ResizeViewport(unsigned int width, unsigned int height)
{
    m_scene->camera->SetAperture((float)width / (float)height);
    m_viewport->Resize(width, height);
}


// =============================================================================
// Callbacks

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    InputManager* inputManager = (InputManager*)glfwGetWindowUserPointer(window);
    inputManager->ResizeViewport(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    InputManager* inputManager = (InputManager*)glfwGetWindowUserPointer(window);
    inputManager->OnMouseMove(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    InputManager* inputManager = (InputManager*)glfwGetWindowUserPointer(window);
    inputManager->OnMouseButton(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    InputManager* inputManager = (InputManager*)glfwGetWindowUserPointer(window);
    inputManager->OnMouseScroll(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    InputManager* inputManager = (InputManager*)glfwGetWindowUserPointer(window);
    inputManager->OnKey(key, scancode, action, mods);
}

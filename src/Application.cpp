#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <vector>

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <ImGuiFileDialog.h>

#include <Buffers.h>
#include <Camera.h>
#include <Primitives.h>
#include <Shader.h>
#include <Scene.h>
#include <Texture.h>
#include <UI.h>

#include <Application.h>


Application::Application()
{
    InitGL();
    if (!IsInitialised())
        return;
    glfwSetWindowUserPointer(window, this);
    SetCallbacks();
}

Application::~Application()
{
    if (window)
        glfwDestroyWindow(window);
    if (m_glfw_initialised)
        glfwTerminate();
}

// Orthographic operations (Matrices aren't working correctly if camera position is changed)
glm::vec2 Application::ScreenToWorldPos(float x, float y)
{
    return glm::vec2(
        scene->camera->Position.x + (2 * x / windowWidth - 1) * (scene->camera->hAperture * scene->camera->Focal),
        scene->camera->Position.y + (2 * (1 - y / windowHeight) - 1) * (scene->camera->vAperture * scene->camera->Focal)
    );
}

glm::vec2 Application::ScreenToWorldOffset(float x, float y)
{
    return glm::vec2(
        (x / windowWidth) * (scene->camera->hAperture * 2 * scene->camera->Focal),
        (y / windowHeight) * (scene->camera->vAperture * 2 * scene->camera->Focal)
    );
}

void Application::SetCamera(Camera* camera)
{
    scene->camera = camera;
    cameraBuffer->SetCamera(scene->camera);
}

void Application::SetWindowSize(int width, int height)
{
    scene->camera->SetAperture((float)width / (float)height);
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

void Application::OnMouseMove(double xpos, double ypos)
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
    for (int i = scene->tokens.size() - 1; i >= 0; i--)
    {
        // Only highlight the first matching token, but unhighlight any others that were highlighted
        // Tokens are drawn from first to last, so iterate in reverse to find the topmost
        Token* token = &scene->tokens[i];
        token->isHighlighted = token->Contains(worldPos) && !highlighted;
        highlighted |= token->isHighlighted;
    }

    if (middleMouseHeld)
        scene->camera->Pan(ScreenToWorldOffset(xoffset, yoffset));
    else if (leftMouseHeld && uiState.selectedTokens.size() > 0)
    {
        if (uiState.snapToGrid)
        {
            for (Token* token : uiState.selectedTokens)
            {
                glm::vec2 center = scene->grid.TokenSnapPosition(token, ScreenToWorldPos(xpos, ypos));
                token->SetPos(glm::vec3(center.x, center.y, token->GetPos().z));
            }
        }
        else
        {
            glm::vec2 offset = ScreenToWorldOffset(xoffset, yoffset);
            for (Token* token : uiState.selectedTokens)
                token->Move(offset);
        }
    }
    else if (leftMouseHeld && uiState.dragSelectRect)
    {
        // GL uses inverted Y-axis
        uiState.dragSelectRect->endCorner = glm::vec2(xpos, windowHeight - ypos);

        // Y-axis is inverted on rect, use re-invert for calculating world positions
        auto selectedTokens = TokensInScreenRect(
            uiState.dragSelectRect->MinX(),
            windowHeight - uiState.dragSelectRect->MinY(),
            uiState.dragSelectRect->MaxX(),
            windowHeight - uiState.dragSelectRect->MaxY()
        );

        for (Token* token : selectedTokens)
            token->isHighlighted = true;
    }
}

void Application::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        middleMouseHeld = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            Token* token = GetTokenAtScreenPos(xpos, ypos);
            if (token && !token->isSelected)
            {
                ClearSelection();
                SelectToken(token);
            }
            // If nothing was immediately selected/being modified, start a drag select
            else if (!token)
            {
                ClearSelection();
                uiState.dragSelectRect = std::make_unique<RectOverlay>();
                // GL uses inverted Y-axis
                uiState.dragSelectRect->startCorner = uiState.dragSelectRect->endCorner = glm::vec2(xpos, windowHeight - ypos);
            }
        }
        else if (action == GLFW_RELEASE && uiState.dragSelectRect)
        {
            // Y-axis is inverted on rect, use re-invert for calculating world positions
            auto selectedTokens = TokensInScreenRect(
                uiState.dragSelectRect->MinX(),
                windowHeight - uiState.dragSelectRect->MinY(),
                uiState.dragSelectRect->MaxX(),
                windowHeight - uiState.dragSelectRect->MaxY()
            );

            for (Token* token : selectedTokens)
            {
                token->isSelected = true;
                uiState.selectedTokens.push_back(token);
            }

            uiState.dragSelectRect.reset();
        }
        leftMouseHeld = action == GLFW_PRESS;
    }
}

void Application::OnMouseScroll(double xoffset, double yoffset)
{
    scene->camera->Zoom(yoffset);
}

void Application::OnKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        uiState.snapToGrid = !uiState.snapToGrid;
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE && uiState.selectedTokens.size() > 0)
    {
        for (Token* token : uiState.selectedTokens)
        {
            TokenGridSize gridSize = static_cast<TokenGridSize>(scene->grid.GetTokenGridSize(token) + 1);
            token->SetSize(scene->grid.SnapGridSize(gridSize));
        }
    }
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE && uiState.selectedTokens.size() > 0)
    {
        for (Token* token : uiState.selectedTokens)
        {
            TokenGridSize gridSize = static_cast<TokenGridSize>(scene->grid.GetTokenGridSize(token) - 1);
            token->SetSize(scene->grid.SnapGridSize(gridSize));
        }
    }
}

bool Application::IsInitialised()
{
    return window && m_glfw_initialised && m_glad_initialised;
}

void Application::Draw()
{
    // TODO: Initialising here to prevent segault on init, this needs a cleaner setup
    CameraBuffer camBuffer = CameraBuffer();
    cameraBuffer = &camBuffer;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // TODO: Shouldn't have to update this every time
        //       Could possibly have a class BufferedCamera : Camera, UniformBuffer
        //       that updates its buffer when changed...?
        cameraBuffer->SetCamera(scene->camera);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        scene->Draw();
        if (uiState.dragSelectRect)
            uiState.dragSelectRect->Draw();
        DrawUI(scene, &uiState);

        glfwSwapBuffers(window);

        // Lazy hack to limit frame rate for now
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// =============================================================================
// Private

void Application::InitGL()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    m_glfw_initialised = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif

    // Create window with graphics context
    window = glfwCreateWindow(windowWidth, windowHeight, "Main Window", NULL, NULL);
    if (window == NULL)
        return;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync


    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    m_glad_initialised = true;
}

void Application::SetCallbacks()
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
}

std::vector<Token*> Application::TokensInScreenRect(float minx, float miny, float maxx, float maxy)
{
    glm::vec2 lo = ScreenToWorldPos(minx, miny);
    glm::vec2 hi = ScreenToWorldPos(maxx, maxy);

    std::vector<Token*> tokens;
    for (Token& token: scene->tokens)
    {
        float radius = token.GetSize() * 0.5f;
        glm::vec3 tokenPos = token.GetPos();
        if (tokenPos.x + radius > lo.x && tokenPos.x - radius < hi.x
            && tokenPos.y + radius > lo.y && tokenPos.y - radius < hi.y)
        {
            tokens.push_back(&token);
        }
    }
    return tokens;
}

void Application::ClearSelection()
{
    for (Token* token : uiState.selectedTokens)
        token->isSelected = false;
    uiState.selectedTokens.clear();
}

void Application::SelectToken(Token* token)
{
    uiState.selectedTokens.push_back(token);
    token->isSelected = true;
}


Token* Application::GetTokenAtScreenPos(float xpos, float ypos)
{
    glm::vec2 worldPos = ScreenToWorldPos(xpos, ypos);
    // Tokens are drawn from first to last, so iterate in reverse to find the topmost
    for (int i = scene->tokens.size() - 1; i >= 0; i--)
    {
        Token* token = &scene->tokens[i];
        // It should only be possible to select one token with a single click
        if (token->Contains(worldPos))
        {
            return token;
        }
    }
    return nullptr;
}

// =============================================================================
// Callbacks

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->SetWindowSize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->OnMouseMove(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->OnMouseButton(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }

    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->OnMouseScroll(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->OnKey(key, scancode, action, mods);
}

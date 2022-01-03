#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>

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


unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;
bool firstMouse = true;
float lastMouseX, lastMouseY;
bool middleMouseHeld = false;
bool leftMouseHeld = false;
float deltaTime, lastFrame = 0.0f;
Scene* scene;
UIState uiState;


// Orthographic operations (Matrices aren't working correctly if camera position is changed)
glm::vec2 ScreenToWorldPos(float x, float y)
{
    return glm::vec2(
        scene->camera->Position.x + (2 * x / windowWidth - 1) * (scene->camera->hAperture * scene->camera->Focal),
        scene->camera->Position.y + (2 * (1 - y / windowHeight) - 1) * (scene->camera->vAperture * scene->camera->Focal)
    );
}

glm::vec2 NearestGridCenter(glm::vec2 pos)
{
    float scale = scene->grid.GetScale();
    float halfScale = scale * 0.5f;
    return glm::vec2(
        int(pos.x / scale) * scale + (pos.x > 0 ? halfScale : -halfScale),
        int(pos.y / scale) * scale + (pos.y > 0 ? halfScale : -halfScale)
    );
}

glm::vec2 ScreenToWorldOffset(float x, float y)
{
    return glm::vec2(
        (x / windowWidth) * (scene->camera->hAperture * 2 * scene->camera->Focal),
        (y / windowHeight) * (scene->camera->vAperture * 2 * scene->camera->Focal)
    );
}

// =============================================================================
// Callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    scene->camera->SetAperture((float)width / (float)height);
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

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
            glm::vec2 center = NearestGridCenter(ScreenToWorldPos(xpos, ypos));
            for (Token* token : uiState.selectedTokens)
                token->SetPos(glm::vec3(center.x, center.y, token->GetPos().z));
        }
        else
        {
            glm::vec2 offset = ScreenToWorldOffset(xoffset, yoffset);
            for (Token* token : uiState.selectedTokens)
                token->Move(offset);
        }
    }
    // else
    //     scene->camera->ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        middleMouseHeld = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            for (Token* token : uiState.selectedTokens)
                token->isSelected = false;
            uiState.selectedTokens.clear();

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glm::vec2 worldPos = ScreenToWorldPos(xpos, ypos);
            // Tokens are drawn from first to last, so iterate in reverse to find the topmost
            for (int i = scene->tokens.size() - 1; i >= 0; i--)
            {
                Token* token = &scene->tokens[i];
                // It should only be possible to select one token with a single click
                if (token->Contains(worldPos))
                {
                    token->isSelected = true;
                    uiState.selectedTokens.push_back(token);
                    break;
                }
            }
        }
        leftMouseHeld = action == GLFW_PRESS;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        std::cout << "Captured" << std::endl;
        return;
    }

    scene->camera->ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        scene->camera->MovementSpeed *= 5;
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        scene->camera->MovementSpeed /= 5;
}

void setCallbacks(GLFWwindow* window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
}

// =============================================================================
// Context Guards

class InitGL
{
public:
    GLFWwindow* window;
    const char* glsl_version = "#version 330";
    
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    InitGL()
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

    ~InitGL()
    {
        if (window)
            glfwDestroyWindow(window);
        if (m_glfw_initialised)
            glfwTerminate();
    }

    bool IsInitialised()
    {
        return window && m_glfw_initialised && m_glad_initialised;
    }

private:
    bool m_glfw_initialised = false;
    bool m_glad_initialised = false;
};


// =============================================================================


void processInput(GLFWwindow *window)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::UP, deltaTime);
}



int main(int, char**)
{
    // Context guards initialise and teardown the GL and ImGui contexts
    InitGL glGuard = InitGL();
    if (!glGuard.IsInitialised())
        return 1;
    setCallbacks(glGuard.window);
    ImGuiContextGuard imguiGuard(glGuard.window, glGuard.glsl_version);

    // Scene
    stbi_set_flip_vertically_on_load(true);

    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), true, 1.0f, windowWidth / windowHeight);
    Scene theScene = Scene(&camera, "resources/images/CaveMap.jpg");
    theScene.AddToken("resources/images/Dragon.jpeg", glm::vec3(0.3f, 0, 0));
    theScene.AddToken("resources/images/Dragon.jpeg", glm::vec3(0.3f, 0.3f, 0));
    theScene.AddToken("resources/images/Dragon.jpeg", glm::vec3(0, 0.3f, 0));

    scene = &theScene;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    // TODO: Initialise
    CameraBuffer cameraBuffer = CameraBuffer(scene->camera);

    // Main loop
    while (!glfwWindowShouldClose(glGuard.window))
    {
        glfwPollEvents();
        // TODO: Shouldn't have to update this every time
        //       Could possibly have a class BufferedCamera : Camera, UniformBuffer
        //       that updates its buffer when changed...?
        cameraBuffer.SetCamera(scene->camera);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        scene->Draw();
        DrawUI(scene, &uiState);

        glfwSwapBuffers(glGuard.window);

        // Lazy hack to limit frame rate for now
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}

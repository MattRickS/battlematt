#pragma once
#include <vector>

#include <GLFW/glfw3.h>
#include <ImGuiFileDialog.h>

#include <Buffers.h>
#include <Camera.h>
#include <Scene.h>
#include <UI.h>


class Application
{
public:
    const char* glsl_version = "#version 330";
    Scene* scene;

    Application();
    ~Application();

    // Orthographic operations (Matrices aren't working correctly if camera position is changed)
    glm::vec2 ScreenToWorldPos(float x, float y);
    glm::vec2 ScreenToWorldOffset(float x, float y);

    void SetCamera(Camera* camera);
    void SetWindowSize(int width, int height);

    GLFWwindow* Window() const { return window; }
    glm::vec2 WindowDimensions() { return glm::vec2(windowWidth, windowHeight); }

    void OnMouseMove(double xpos, double ypos);
    void OnMouseButton(int button, int action, int mods);
    void OnMouseScroll(double xoffset, double yoffset);
    void OnKey(int key, int scancode, int action, int mods);

    bool IsInitialised();
    void Draw();

private:
    GLFWwindow* window;
    bool m_glfw_initialised = false;
    bool m_glad_initialised = false;

    unsigned int windowWidth = 1280;
    unsigned int windowHeight = 720;
    bool firstMouse = true;
    float lastMouseX, lastMouseY;
    bool middleMouseHeld = false;
    bool leftMouseHeld = false;
    float deltaTime, lastFrame = 0.0f;
    UIState uiState;
    CameraBuffer* cameraBuffer;

    void InitGL();
    void SetCallbacks();

    std::vector<Token*> TokensInScreenRect(float minx, float miny, float maxx, float maxy);
};

// =============================================================================
// Callbacks

void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

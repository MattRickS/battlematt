#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <view/Window.h>

// =============================================================================
// Callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Window* window_ = (Window*)glfwGetWindowUserPointer(window);
    window_->Resize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Window* window_ = (Window*)glfwGetWindowUserPointer(window);
    window_->OnMouseMoved(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Window* window_ = (Window*)glfwGetWindowUserPointer(window);
    window_->OnMouseButtonChanged(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Window* window_ = (Window*)glfwGetWindowUserPointer(window);
    window_->OnMouseScrolled(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* window_ = (Window*)glfwGetWindowUserPointer(window);
    window_->OnKeyChanged(key, scancode, action, mods);
}

void close_callback(GLFWwindow* window)
{
    Window* window_ = (Window*)glfwGetWindowUserPointer(window);
    window_->OnCloseRequested();
}

// =============================================================================

Window::Window(unsigned int width, unsigned int height, const char* name, std::shared_ptr<Window> share) : m_width(width), m_height(height)
{
    window = glfwCreateWindow(width, height, name, NULL, share ? share->window : NULL);
    if (window == NULL)
        return;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    
    monitor = glfwGetPrimaryMonitor();

    // Reference: https://www.glfw.org/docs/3.3/window_guide.html
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowCloseCallback(window, close_callback);
}

Window::~Window()
{
    DisconnectSignals();
    if (window)
        glfwDestroyWindow(window);
}

bool Window::IsInitialised() { return bool(window); }

void Window::Resize(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    sizeChanged.emit(this, m_width, m_height);
}

unsigned int Window::Height() { return m_height; }
unsigned int Window::Width() { return m_width; }

bool Window::IsFullscreen() { return glfwGetWindowMonitor(window) != nullptr; }
void Window::SetFullscreen(bool fullscreen)
{
    if (IsFullscreen() == fullscreen)
        return;

    if (fullscreen)
    {
        // backup window position and window size
        glfwGetWindowPos(window, &m_lastPos[0], &m_lastPos[1]);
        glfwGetWindowSize(window, &m_lastSize[0], &m_lastSize[1]);
        
        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
    }
    else
    {
        // restore last window size and position
        glfwSetWindowMonitor(window, nullptr,  m_lastPos[0], m_lastPos[1], m_lastSize[0], m_lastSize[1], 0);
    }
}

void Window::Show() { glfwShowWindow(window); }
void Window::Hide() { glfwHideWindow(window); }
void Window::Focus() { glfwFocusWindow(window); }

glm::vec2 Window::CursorPos()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(xpos, ypos);
}

void Window::Close()
{
    glfwSetWindowShouldClose(window, true);
}
bool Window::IsClosed() { return glfwWindowShouldClose(window); }

void Window::Draw() {}
void Window::Render()
{
    glfwMakeContextCurrent(window);
    glViewport(0, 0, m_width, m_height);
    Draw();
    glfwSwapBuffers(window);
}

void Window::CopyToClipboard(const std::string& text)
{
    glfwSetClipboardString(window, text.c_str());
}

std::string Window::GetClipboard()
{
    return glfwGetClipboardString(window);
}

void Window::DisconnectSignals()
{
    cursorMoved.disconnect();
    mouseButtonChanged.disconnect();
    mouseScrolled.disconnect();
    keyChanged.disconnect();
    sizeChanged.disconnect();
}

bool Window::HasKeyPressed(int key) { return glfwGetKey(window, key) == GLFW_PRESS; }

// =============================================================================
// Callbacks

void Window::OnMouseMoved(double xpos, double ypos) { cursorMoved.emit(this, xpos, ypos); }
void Window::OnMouseButtonChanged(int button, int action, int mods) { mouseButtonChanged.emit(this, button, action, mods); }
void Window::OnMouseScrolled(double xoffset, double yoffset) { mouseScrolled.emit(this, xoffset, yoffset); }
void Window::OnKeyChanged(int key, int scancode, int action, int mods) { keyChanged.emit(this, key, scancode, action, mods); }
void Window::OnWindowResized(int width, int height)
{
    Resize(width, height);
    sizeChanged.emit(this, width, height);
}
void Window::OnCloseRequested()
{
    // Prevent the window from actually closing and propagate the decision to listeners of the signal
    glfwSetWindowShouldClose(window, GLFW_FALSE);
    closeRequested.emit(this);
}

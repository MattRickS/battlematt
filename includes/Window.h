#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Window
{
public:
    Window(unsigned int width, unsigned int height, const char* name);
    ~Window();

    void Enable();
    void EnableVSync(bool enable);
    void Resize(unsigned int width, unsigned int height);
    unsigned int Height();
    unsigned int Width();
    GLFWwindow* ID();
    glm::vec2 CursorPos();
    bool ShouldClose();
    void Close();
    void SwapBuffers();
    virtual void Draw();

protected:
    GLFWwindow* window;
    unsigned int m_width, m_height;
};

Window::Window(unsigned int width, unsigned int height, const char* name) : m_width(width), m_height(height)
{
    // Create window with graphics context
    window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (window == NULL)
        return;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
}

Window::~Window()
{

    if (window)
        glfwDestroyWindow(window);
}

void Window::Enable()
{
    glfwMakeContextCurrent(window);
}

void Window::EnableVSync(bool enable)
{
    Enable();
    glfwSwapInterval(enable);
}

void Window::Resize(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    Enable();
    glViewport(0, 0, width, height);
}

unsigned int Window::Height() { return m_height; }
unsigned int Window::Width() { return m_width; }
GLFWwindow* Window::ID() { return window; }

glm::vec2 Window::CursorPos()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(xpos, ypos);
}

bool Window::ShouldClose() { glfwWindowShouldClose(window); }
void Window::Close() { glfwSetWindowShouldClose(window, true); }
void Window::SwapBuffers() { glfwSwapBuffers(window); }
void Window::Draw()
{
    glfwSwapBuffers(window);
}

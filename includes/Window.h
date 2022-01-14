#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Signal.hpp>


class Window
{
public:
    Signal<double, double> cursorMoved;
    Signal<int, int, int> mouseButtonChanged;
    Signal<double, double> mouseScrolled;
    Signal<int, int, int, int> keyChanged;
    Signal<int, int> sizeChanged;

    Window(unsigned int width, unsigned int height, const char* name);
    ~Window();

    void Close();
    bool IsClosing();
    bool IsInitialised();

    void Resize(unsigned int width, unsigned int height);
    unsigned int Height();
    unsigned int Width();

    glm::vec2 CursorPos();

    void Render();
    virtual void Draw();

    virtual void OnMouseMoved(double xpos, double ypos);
    virtual void OnMouseButtonChanged(int button, int action, int mods);
    virtual void OnMouseScrolled(double xoffset, double yoffset);
    virtual void OnKeyChanged(int key, int scancode, int action, int mods);
    virtual void OnWindowResized(int width, int height);

protected:
    GLFWwindow* window;
    unsigned int m_width, m_height;

    void DisconnectSignals();
};

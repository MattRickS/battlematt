#pragma once
#include <memory>

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
    Signal<> closeRequested;

    Window(unsigned int width, unsigned int height, const char* name, std::shared_ptr<Window> share = NULL);
    ~Window();

    void Focus();
    void Close();
    bool IsClosed();
    bool IsInitialised();

    void Resize(unsigned int width, unsigned int height);
    unsigned int Height();
    unsigned int Width();
    bool IsFullscreen();
    void SetFullscreen(bool fullscreen);

    glm::vec2 CursorPos();

    void Render();
    virtual void Draw();

    virtual void OnMouseMoved(double xpos, double ypos);
    virtual void OnMouseButtonChanged(int button, int action, int mods);
    virtual void OnMouseScrolled(double xoffset, double yoffset);
    virtual void OnKeyChanged(int key, int scancode, int action, int mods);
    virtual void OnWindowResized(int width, int height);
    virtual void OnCloseRequested();

protected:
    GLFWwindow* window;
    GLFWmonitor* monitor = nullptr;
    unsigned int m_width, m_height;
    int m_lastPos[2], m_lastSize[2];

    void DisconnectSignals();
};

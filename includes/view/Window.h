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

    void Use();

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

    void CopyToClipboard(const std::string& text);
    std::string GetClipboard();

    bool HasKeyPressed(int key);

    virtual void OnMouseMoved(double xpos, double ypos);
    virtual void OnMouseButtonChanged(int button, int action, int mods);
    virtual void OnMouseScrolled(double xoffset, double yoffset);
    virtual void OnKeyChanged(int key, int scancode, int action, int mods);
    virtual void OnWindowResized(int width, int height);
    virtual void OnCloseRequested();

    GLFWwindow* window;
protected:
    GLFWmonitor* monitor = nullptr;
    unsigned int m_width, m_height;
    int m_lastPos[2], m_lastSize[2];

    void DisconnectSignals();
};

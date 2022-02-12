#pragma once

#include <GLFW/glfw3.h>

class RenderBuffer
{
public:
    RenderBuffer(int width, int height);
    ~RenderBuffer();

    void Resize(int width, int height);

protected:
    GLuint ID;
    int width, height;
};

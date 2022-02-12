#pragma once
#include <memory>
#include <unordered_map>

#include <GLFW/glfw3.h>

#include <view/RenderBuffer.h>
#include <view/Window.h>

enum class BufferType
{
    Read       = 1 << 1,
    Write      = 1 << 2,
    ReadWrite  = Read | Write
};

class SharedRenderBuffer : public RenderBuffer
{
public:
    SharedRenderBuffer(int width, int height);
    ~SharedRenderBuffer();
    bool Add(const std::shared_ptr<Window>& window, BufferType type);
    void Use(const std::shared_ptr<Window>& window);
    void CopyToBuffer(const std::shared_ptr<Window>& window, GLuint buffer = 0);

private:
    std::unordered_map<std::shared_ptr<Window>, GLuint> framebuffers;
};

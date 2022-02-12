#include <iostream>

#include <glad/glad.h>

#include <view/SharedRenderBuffer.h>


SharedRenderBuffer::SharedRenderBuffer(int width, int height) : RenderBuffer(width, height) {}

SharedRenderBuffer::~SharedRenderBuffer()
{
    for (const auto& it: framebuffers)
        glDeleteFramebuffers(1, &it.second);
}

bool SharedRenderBuffer::Add(const std::shared_ptr<Window>& window, BufferType type)
{
    window->Use();
    GLuint FBO;
    glGenFramebuffers(1, &FBO);
    switch (type)
    {
    case BufferType::Read:
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        break;
    case BufferType::Write:
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
        break;
    case BufferType::ReadWrite:
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        break;
    default:
        glDeleteFramebuffers(1, &FBO);
        std::cerr << "Invalid BufferType" << std::endl;
        return false;
    }
    glNamedFramebufferRenderbuffer(FBO, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ID);
    framebuffers[window] = FBO;
    return true;
}

void SharedRenderBuffer::Use(const std::shared_ptr<Window>& window)
{
    // TODO: Check if in mapping before using
    // TODO: Check if valid as a write BufferType
    window->Use();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[window]);
    glViewport(0, 0, width, height);
}

void SharedRenderBuffer::CopyToBuffer(const std::shared_ptr<Window>& window, GLuint buffer)
{
    // TODO: Check if valid as a write BufferType
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[window]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer);
    glViewport(0, 0, width, height);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
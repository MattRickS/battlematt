#include <glad/glad.h>

#include <view/RenderBuffer.h>


RenderBuffer::RenderBuffer(int width, int height) : width(width), height(height)
{
    glCreateRenderbuffers(1, &ID);
    // glBindRenderbuffer(GL_RENDERBUFFER, ID);
    glNamedRenderbufferStorage(ID, GL_RGBA, width, height);
}

RenderBuffer::~RenderBuffer()
{
    glDeleteRenderbuffers(1, &ID);
}

void RenderBuffer::Resize(int width, int height)
{
    glBindRenderbuffer(GL_RENDERBUFFER, ID);
    glNamedRenderbufferStorage(ID, GL_RGBA, width, height);
}
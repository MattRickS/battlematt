#pragma once
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Camera.h>


class UniformBuffer
{
public:
    GLuint ID;
    uint bindPoint;

    UniformBuffer(uint numBytes, uint bindingPoint = 0) : bindPoint(bindingPoint)
    {
        glGenBuffers(1, &ID);
        
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
        glBufferData(GL_UNIFORM_BUFFER, numBytes, NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, ID, 0, numBytes);
    }
    // 2 * sizeof(glm::mat4)
    void bind(void* data, size_t size, uint offset=0)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
};


const size_t STD140_MATRIX_SIZE = sizeof(glm::mat4);

/*
Provides a uniform buffer matching the following layout

layout(std140, binding=0) uniform Camera
{
    mat4 projection;
    mat4 projectionInv;
    mat4 view;
    mat4 viewInv;
} camera;
*/
class CameraBuffer : public UniformBuffer
{
public:
    CameraBuffer() : UniformBuffer(4 * STD140_MATRIX_SIZE) {}
    CameraBuffer(std::shared_ptr<Camera> camera) : CameraBuffer()
    {
        SetCamera(camera);
    }

    void UpdateProjection()
    {
        bind(glm::value_ptr(m_camera->projectionMatrix), STD140_MATRIX_SIZE);
        bind(glm::value_ptr(m_camera->invProjectionMatrix), STD140_MATRIX_SIZE, STD140_MATRIX_SIZE);
    }

    void UpdateView()
    {
        bind(glm::value_ptr(m_camera->viewMatrix), STD140_MATRIX_SIZE, 2 * STD140_MATRIX_SIZE);
        bind(glm::value_ptr(m_camera->invViewMatrix), STD140_MATRIX_SIZE, 3 * STD140_MATRIX_SIZE);
    }

    void SetCamera(std::shared_ptr<Camera> camera)
    {
        m_camera = camera;
        UpdateProjection();
        UpdateView();
    }

private:
    std::shared_ptr<Camera> m_camera;
};
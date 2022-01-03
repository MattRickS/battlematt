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


class CameraBuffer : public UniformBuffer
{
public:
    CameraBuffer(Camera* camera) : UniformBuffer(2 * sizeof(glm::mat4))
    {
        SetCamera(camera);
    }

    void UpdateView()
    {
        bind(glm::value_ptr(camera->viewMatrix), sizeof(glm::mat4), sizeof(glm::mat4));
    }

    void UpdateProjection()
    {
        bind(glm::value_ptr(camera->projectionMatrix), sizeof(glm::mat4));
    }

    void SetCamera(Camera* camera)
    {
        this->camera = camera;
        UpdateView();
        UpdateProjection();
    }

private:
    Camera* camera;
};
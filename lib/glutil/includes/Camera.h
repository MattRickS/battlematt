

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float FOCAL       =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // euler Angles
    float Yaw = YAW;
    float Pitch = PITCH;
    // camera options
    float MovementSpeed = SPEED;
    float MouseSensitivity = SENSITIVITY;
    float Focal = FOCAL;

    float hAperture = 1.77f;
    float vAperture = 1.0f;
    float near = 0.1f;
    float far = 100.0f;

    bool isOrtho = false;
    glm::mat4 projectionMatrix;
    glm::mat4 invProjectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 invViewMatrix;

    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f),
        bool isOrtho = false,
        float focal = 3.5f,
        float haperture = 1.77f,
        float vaperture = 1.0f,
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)
    );
    void Pan(glm::vec2 offset);
    void Pan(float xoffset, float yoffset);
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void SetAperture(float haperture);

private:
    void updateCameraVectors();
    void updateProjectionMatrix();
    void updateViewMatrix();
};
#endif




#ifndef CAMERA_HPP
#define CAMERA_HPP

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

    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 direction = glm::vec3(0.0f, .0f, -1.0f),
        bool isOrtho = false,
        float focal = 35.0f,
        float haperture = 1.77f,
        float vaperture = 1.0f,
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)
    )
        : Front(direction), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Focal(focal), isOrtho(isOrtho)
    {
        Position = position;
        WorldUp = up;

        // Convert forward direction to euler angles
        float pitchRadians = asin(Front.y);
        float yawRadians = acos(Front.x / cos(pitchRadians));
        Yaw = glm::degrees(yawRadians);
        Pitch = glm::degrees(pitchRadians);

        updateCameraVectors();
        updateProjectionMatrix();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void Pan(float xoffset, float yoffset)
    {
        Position = glm::vec3(Position.x + xoffset, Position.y, Position.z + yoffset);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Focal *= (1.0f - yoffset * 0.1f);
        if (Focal < 0.01f)
            Focal = 0.01f;
        if (Focal > 45.0f)
            Focal = 45.0f;
        updateProjectionMatrix();
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }

    void updateProjectionMatrix()
    {
        if (isOrtho)
            projectionMatrix = glm::ortho(-hAperture * Focal, hAperture * Focal, -vAperture * Focal, vAperture * Focal, near, far);
        else
            projectionMatrix = glm::perspective(2 * glm::atan(hAperture / Focal), hAperture/vAperture, near, far);
    }
};
#endif


#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Camera.h>


Camera::Camera(glm::vec3 position, glm::vec3 direction, bool isOrtho, float focal, float haperture, float vaperture, glm::vec3 up)
    : Position(position), Front(direction), WorldUp(up), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Focal(focal), isOrtho(isOrtho)
{
    // Convert forward direction to euler angles
    float pitchRadians = asin(direction.y);
    float yawRadians;
    if (direction.z == 0)
        yawRadians = acos(direction.x / cos(pitchRadians));
    else
        yawRadians = asin(direction.z / cos(pitchRadians));
    Yaw = glm::degrees(yawRadians);
    Pitch = glm::degrees(pitchRadians);

    updateCameraVectors();
    updateProjectionMatrix();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::Pan(float xoffset, float yoffset)
{
    Position -= Right * xoffset + Up * yoffset;
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
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
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
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
void Camera::ProcessMouseScroll(float yoffset)
{
    Focal *= (1.0f - yoffset * 0.1f);
    if (Focal < 0.01f)
        Focal = 0.01f;
    if (Focal > 45.0f)
        Focal = 45.0f;
    updateProjectionMatrix();
}

void Camera::SetAperture(float haperture)
{
    hAperture = haperture;
    updateProjectionMatrix();
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
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

void Camera::updateProjectionMatrix()
{
    if (isOrtho)
        projectionMatrix = glm::ortho(-hAperture * Focal, hAperture * Focal, -vAperture * Focal, vAperture * Focal, near, far);
    else
        projectionMatrix = glm::perspective(2 * glm::atan(hAperture / Focal), hAperture/vAperture, near, far);
}


#include "Camera.h"
#include <glm/gtc/quaternion.hpp>

namespace Donut
{
    Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane)
    {
        RecalculateProjectionMatrix();
        RecalculateViewMatrix();
    }

    void Camera::SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane)
    {
        m_FOV = fov;
        m_AspectRatio = aspectRatio;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        RecalculateProjectionMatrix();
    }

    void Camera::RecalculateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void Camera::RecalculateViewMatrix()
    {
        float pitch = glm::radians(m_Rotation.x);
        float yaw   = glm::radians(m_Rotation.y);
        float roll  = glm::radians(m_Rotation.z);

        glm::vec3 direction;
        direction.x = cos(yaw) * cos(pitch);
        direction.y = sin(pitch);
        direction.z = sin(yaw) * cos(pitch);

        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        glm::vec3 front = glm::normalize(direction);
        glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
        glm::vec3 up    = glm::normalize(glm::cross(right, front));

        m_ViewMatrix           = glm::lookAt(m_Position, m_Position + front, up);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    glm::vec3 Camera::GetForwardDirection() const
    {
        float pitch = glm::radians(m_Rotation.x);
        float yaw   = glm::radians(m_Rotation.y);

        glm::vec3 direction;
        direction.x = cos(yaw) * cos(pitch);
        direction.y = sin(pitch);
        direction.z = sin(yaw) * cos(pitch);

        return glm::normalize(direction);
    }

    glm::vec3 Camera::GetRightDirection() const
    {
        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        return glm::normalize(glm::cross(GetForwardDirection(), worldUp));
    }

    glm::vec3 Camera::GetUpDirection() const
    {
        return glm::normalize(glm::cross(GetRightDirection(), GetForwardDirection()));
    }

    void Camera::OnMouseMove(float xOffset, float yOffset, bool constrainPitch)
    {
        xOffset *= m_MouseSensitivity;
        yOffset *= m_MouseSensitivity;

        m_Rotation.y += xOffset;
        m_Rotation.x += yOffset;

        if (constrainPitch)
        {
            if (m_Rotation.x > 89.0f)
                m_Rotation.x = 89.0f;
            if (m_Rotation.x < -89.0f)
                m_Rotation.x = -89.0f;
        }

        RecalculateViewMatrix();
    }

    void Camera::MoveForward(float deltaTime)
    {
        m_Position += GetForwardDirection() * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void Camera::MoveBackward(float deltaTime)
    {
        m_Position -= GetForwardDirection() * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void Camera::MoveRight(float deltaTime)
    {
        m_Position += GetRightDirection() * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void Camera::MoveLeft(float deltaTime)
    {
        m_Position -= GetRightDirection() * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void Camera::MoveUp(float deltaTime)
    {
        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        m_Position += worldUp * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }

    void Camera::MoveDown(float deltaTime)
    {
        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        m_Position -= worldUp * m_MovementSpeed * deltaTime;
        RecalculateViewMatrix();
    }
}

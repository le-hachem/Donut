#include "Camera.h"

#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>

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
        if (m_CameraMode == CameraMode::FPS)
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
        else if (m_CameraMode == CameraMode::Orbital)
        {
            glm::vec3 position = GetOrbitalPosition();
            glm::vec3 target = m_OrbitalTarget;
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            
            m_ViewMatrix = glm::lookAt(position, target, up);
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }
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
        if (m_CameraMode == CameraMode::FPS)
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
    }

    void Camera::MoveForward(float deltaTime)
    {
        if (m_CameraMode == CameraMode::FPS)
        {
            m_Position += GetForwardDirection() * m_MovementSpeed * deltaTime;
            RecalculateViewMatrix();
        }
    }

    void Camera::MoveBackward(float deltaTime)
    {
        if (m_CameraMode == CameraMode::FPS)
        {
            m_Position -= GetForwardDirection() * m_MovementSpeed * deltaTime;
            RecalculateViewMatrix();
        }
    }

    void Camera::MoveRight(float deltaTime)
    {
        if (m_CameraMode == CameraMode::FPS)
        {
            m_Position += GetRightDirection() * m_MovementSpeed * deltaTime;
            RecalculateViewMatrix();
        }
    }

    void Camera::MoveLeft(float deltaTime)
    {
        if (m_CameraMode == CameraMode::FPS)
        {
            m_Position -= GetRightDirection() * m_MovementSpeed * deltaTime;
            RecalculateViewMatrix();
        }
    }

    void Camera::MoveUp(float deltaTime)
    {
        if (m_CameraMode == CameraMode::FPS)
        {
            glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
            m_Position += worldUp * m_MovementSpeed * deltaTime;
            RecalculateViewMatrix();
        }
    }

    void Camera::MoveDown(float deltaTime)
    {
        if (m_CameraMode == CameraMode::FPS)
        {
            glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
            m_Position -= worldUp * m_MovementSpeed * deltaTime;
            RecalculateViewMatrix();
        }
    }

    glm::vec3 Camera::GetOrbitalPosition() const
    {
        float clampedElevation = glm::clamp(m_Elevation, 0.01f, float(std::numbers::pi) - 0.01f);
        return glm::vec3
        (
            m_OrbitalRadius * sin(clampedElevation) * cos(m_Azimuth),
            m_OrbitalRadius * cos(clampedElevation),
            m_OrbitalRadius * sin(clampedElevation) * sin(m_Azimuth)
        );
    }

    void Camera::UpdateOrbital()
    {
        m_OrbitalTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        if (m_Dragging || m_Panning)
            m_Moving = true;
        else
            m_Moving = false;
        RecalculateViewMatrix();
    }

    void Camera::ProcessOrbitalMouseMove(double x, double y)
    {
        if (m_Dragging && !m_Panning)
        {
            float dx = float(x - m_LastX_Orbital);
            float dy = float(y - m_LastY_Orbital);
            
            m_Azimuth   += dx * m_OrbitalSpeed;
            m_Elevation -= dy * m_OrbitalSpeed;
            m_Elevation = glm::clamp(m_Elevation, 0.01f, float(std::numbers::pi) - 0.01f);
        }
        
        m_LastX_Orbital = x;
        m_LastY_Orbital = y;
        UpdateOrbital();
    }

    void Camera::ProcessOrbitalMouseButton(int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                m_Dragging = true;
                m_Panning = false;
            }
            else if (action == GLFW_RELEASE)
            {
                m_Dragging = false;
                m_Panning = false;
            }
        }
    }

    void Camera::ProcessOrbitalScroll(double xoffset, double yoffset)
    {
        m_OrbitalRadius -= yoffset * m_ZoomSpeed;
        m_OrbitalRadius = glm::clamp(m_OrbitalRadius, m_OrbitalMinRadius, m_OrbitalMaxRadius);
        UpdateOrbital();
    }
}

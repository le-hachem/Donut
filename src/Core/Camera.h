#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Donut
{
    class Camera
    {
    public:
        Camera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, 
               float nearPlane = 0.1f, float farPlane = 100.0f);
        ~Camera() = default;

        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
        void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
        
        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::vec3& GetRotation() const { return m_Rotation; }
        
        glm::vec3 GetForwardDirection() const;
        glm::vec3 GetRightDirection()   const;
        glm::vec3 GetUpDirection()      const;

        const glm::mat4& GetProjectionMatrix()     const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix()           const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        void SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
        void OnMouseMove(float xOffset, float yOffset, bool constrainPitch = true);
        
        void MoveForward(float deltaTime);
        void MoveBackward(float deltaTime);
        void MoveRight(float deltaTime);
        void MoveLeft(float deltaTime);
        void MoveUp(float deltaTime);
        void MoveDown(float deltaTime);

        void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
        float GetMouseSensitivity() const  { return m_MouseSensitivity; }

        void SetMovementSpeed(float speed) { m_MovementSpeed = speed;   }
        float GetMovementSpeed()    const  { return m_MovementSpeed;    }

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();
    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position = { 0.0f, 0.0f, 3.0f };
        glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };

        float m_FOV         = 45.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        float m_NearPlane   = 0.1f;
        float m_FarPlane    = 100.0f;

        float m_MouseSensitivity = 0.1f;
        float m_MovementSpeed    = 5.0f;
        bool  m_FirstMouse       = true;
        float m_LastX            = 0.0f;
        float m_LastY            = 0.0f;
    };
}

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <numbers>

namespace Donut
{
    enum class CameraMode
    {
        FPS,
        Orbital
    };

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

        void SetOrbitalTarget(const glm::vec3& target) { m_OrbitalTarget = target; }
        const glm::vec3& GetOrbitalTarget()      const { return m_OrbitalTarget;   }
        
        void SetOrbitalRadius(double radius) { m_OrbitalRadius = radius; }
        double GetOrbitalRadius() const { return m_OrbitalRadius; }
        
        void SetOrbitalLimits(double minRadius, double maxRadius) 
        { 
            m_OrbitalMinRadius = minRadius; 
            m_OrbitalMaxRadius = maxRadius; 
        }
        
        void SetOrbitalSpeed(float speed) { m_OrbitalSpeed = speed; }
        float GetOrbitalSpeed() const { return m_OrbitalSpeed; }
        
        void SetZoomSpeed(double speed) { m_ZoomSpeed = speed; }
        double GetZoomSpeed() const { return m_ZoomSpeed; }
        
        void SetAzimuth(float azimuth) { m_Azimuth = azimuth; }
        float GetAzimuth() const { return m_Azimuth; }
        
        void SetElevation(float elevation) { m_Elevation = elevation; }
        float GetElevation() const { return m_Elevation; }
        
        glm::vec3 GetOrbitalPosition() const;
        void UpdateOrbital();
        void ProcessOrbitalMouseMove(double x, double y);
        void ProcessOrbitalMouseButton(int button, int action, int mods);
        void ProcessOrbitalScroll(double xoffset, double yoffset);
        
        void       SetCameraMode(CameraMode mode) { m_CameraMode = mode; }
        CameraMode GetCameraMode()          const { return m_CameraMode; }
        
        bool  IsDragging() const { return m_Dragging; }
        bool  IsPanning()  const { return m_Panning;  }
        bool  IsMoving()   const { return m_Moving;   }
        double GetLastX()  const { return m_LastX;    }
        double GetLastY()  const { return m_LastY;    }

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();
    private:
        CameraMode m_CameraMode = CameraMode::FPS;
        
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
        
        glm::vec3 m_OrbitalTarget    = glm::vec3(0.0f, 0.0f, 0.0f);
        double    m_OrbitalRadius    = 6.34194e10;
        double    m_OrbitalMinRadius = 1e10;
        double    m_OrbitalMaxRadius = 1e12;
        float     m_Azimuth          = 0.0f;
        float     m_Elevation        = static_cast<float>(std::numbers::pi) / 2.0f;
        float     m_OrbitalSpeed     = 0.01f;
        double    m_ZoomSpeed        = 25e9f;
        bool      m_Dragging         = false;
        bool      m_Panning          = false;
        bool      m_Moving           = false;
        double    m_LastX_Orbital    = 0.0;
        double    m_LastY_Orbital    = 0.0;
    };
}

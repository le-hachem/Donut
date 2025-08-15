#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>

#include "Core/Application.h"
#include "Core/Camera.h"
#include "Rendering/Renderer.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexArray.h"
#include "Rendering/Texture.h"
#include "Rendering/UniformBuffer.h"
#include "Rendering/TextureManager.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Donut
{
    const double c = 299792458.0;
    const double G = 6.67430e-11;

    struct BlackHole
    {
        glm::vec3 m_Position;
        double m_Mass;
        double m_Radius;
        double m_Rs;

        BlackHole(glm::vec3 pos, float mass) 
            : m_Position(pos), m_Mass(mass)
        {
            m_Rs = 2.0 * G * m_Mass / (c * c);
        }

        bool Intercept(float px, float py, float pz) const
        {
            double dx = double(px) - double(m_Position.x);
            double dy = double(py) - double(m_Position.y);
            double dz = double(pz) - double(m_Position.z);
            double dist2 = dx * dx + dy * dy + dz * dz;
            return dist2 < m_Rs * m_Rs;
        }
    };

    struct ObjectData
    {
        glm::vec4 m_PosRadius;
        glm::vec4 m_Color;
        float     m_Mass;
        glm::vec3 m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    class Engine
    {
    public:
        Engine();
        ~Engine() = default;

        void DrawFullScreenQuad();
        void DispatchCompute(const Camera& cam);
        void UploadCameraUBO(const Camera& cam);
        void UploadObjectsUBO(const std::vector<ObjectData>& objs);
        void UploadDiskUBO();
        void RenderScene();
        void UpdatePhysics(float deltaTime);
        void UpdateWindowDimensions();

        int GetWidth()  const { return m_Width;  }
        int GetHeight() const { return m_Height; }

        std::vector<ObjectData>& GetObjects() { return m_Objects; }
        BlackHole&               GetSagA()    { return m_SagA;    }
        Camera&                  GetCamera()  { return m_Camera;  }
        bool&                    GetGravity() { return m_Gravity; }

        void  UpdatePerformance(float deltaTime);
        void  SetTargetFPS(int fps)        { m_TargetFPS = fps;                             }
        int   GetTargetFPS()         const { return m_TargetFPS;                            }
        float GetCurrentFPS()        const { return m_CurrentFPS;                           }
        void  SetComputeHeight(int height) { m_ComputeHeight = height;                      }
        int   GetComputeHeight()     const { return m_ComputeHeight;                        }
        int   GetComputeWidth()      const { return (m_Width * m_ComputeHeight) / m_Height; }
        void  UpdateComputeDimensions();
    private:
        Ref<Shader> CreateComputeProgram(const char* path);
        std::pair<Ref<VertexArray>, Ref<Texture2D>> QuadVAO();
    private:
        Ref<VertexArray>   m_QuadVAO;
        Ref<Texture2D>     m_Texture;
        Ref<Shader>        m_ShaderProgram;
        Ref<Shader>        m_ComputeProgram;
        Ref<UniformBuffer> m_CameraUBO;
        Ref<UniformBuffer> m_DiskUBO;
        Ref<UniformBuffer> m_ObjectsUBO;

        int   m_Width;
        int   m_Height;
        float m_Width_f = 100*10e10f;
        float m_Height_f = 75*10e10f;

        int   m_TargetFPS     = 60;
        float m_CurrentFPS    = 60.0f;
        float m_LastFrameTime = 0.0f;
        int   m_ComputeHeight = 320;

        std::vector<ObjectData> m_Objects;
        BlackHole               m_SagA;
        Camera                  m_Camera;
        bool                    m_Gravity = false;
    };
};

#pragma once

#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>

#include "Core/Application.h"
#include "Core/Camera.h"
#include "Rendering/Renderer.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"
#include "Rendering/VertexArray.h"
#include "Rendering/UniformBuffer.h"

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
            : m_Position(pos),
              m_Mass(mass) 
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
        float m_Mass;
        glm::vec3 m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    class Engine
    {
    public:
        Engine();
        ~Engine() = default;

        void GenerateGrid(const std::vector<ObjectData>& objects);
        void DrawGrid(const glm::mat4& viewProj);
        void DrawFullScreenQuad();
        void DispatchCompute(const Camera& cam);
        void UploadCameraUBO(const Camera& cam);
        void UploadObjectsUBO(const std::vector<ObjectData>& objs);
        void UploadDiskUBO();
        void RenderScene();
        void UpdatePhysics(float deltaTime);
        void UpdateWindowDimensions();

              Camera& GetCamera()       { return m_Camera; }
        const Camera& GetCamera() const { return m_Camera; }
        
        const std::vector<ObjectData>& GetObjects() const { return m_Objects; }
              std::vector<ObjectData>& GetObjects()       { return m_Objects; }
        
        bool GetGravity() const       { return m_Gravity;    }
        void SetGravity(bool gravity) { m_Gravity = gravity; }
        
        int GetWidth()         const { return m_Width;         }
        int GetHeight()        const { return m_Height;        }
        int GetComputeWidth()  const { return m_ComputeWidth;  }
        int GetComputeHeight() const { return m_ComputeHeight; }

    private:
        std::string CreateBasicVertexShader();
        std::string CreateBasicFragmentShader();
        std::string LoadComputeShader(const std::string& path);
    private:
        Ref<Shader>        m_GridShader;
        Ref<Shader>        m_QuadShader;
        Ref<Shader>        m_ComputeShader;
        Ref<Texture2D>     m_Texture;
        Ref<VertexArray>   m_QuadVAO;
        Ref<VertexArray>   m_GridVAO;
        Ref<UniformBuffer> m_CameraUBO;
        Ref<UniformBuffer> m_DiskUBO;
        Ref<UniformBuffer> m_ObjectsUBO;
        
        int   m_GridIndexCount = 0;
        int   m_Width;
        int   m_Height;
        int   m_ComputeWidth;
        int   m_ComputeHeight;
        float m_WidthScale = 100000000000.0f;
        float m_HeightScale = 75000000000.0f;

        std::vector<ObjectData> m_Objects;
        BlackHole               m_SagA;
        Camera                  m_Camera;
        bool                    m_Gravity = false;
    };
}

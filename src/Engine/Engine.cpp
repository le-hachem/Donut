#include <iostream>
#include <fstream>
#include <sstream>

#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"

namespace Donut
{
    Engine::Engine()
        : m_SagA(glm::vec3(0.0f, 0.0f, 0.0f), 8.54e36f)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        glfwGetFramebufferSize(window, &m_Width, &m_Height);
        
        m_Camera.SetCameraMode(CameraMode::Orbital);
        m_Camera.SetOrbitalRadius(6.34194e10);
        m_Camera.SetOrbitalLimits(1e10, 1e12);
        m_Camera.SetOrbitalSpeed(0.01f);
        m_Camera.SetZoomSpeed(25e9f);
        
        m_Objects = 
        {
            { glm::vec4(4e11f, 0.00f, 0.00f, 4e10f),       glm::vec4(1, 1, 0, 1), static_cast<float>(1.98892e30)    },
            { glm::vec4(0.00f, 0.00f, 4e11f, 4e10f),       glm::vec4(1, 0, 0, 1), static_cast<float>(1.98892e30)    },
            { glm::vec4(0.00f, 0.00f, 0.00f, m_SagA.m_Rs), glm::vec4(0, 0, 0, 1), static_cast<float>(m_SagA.m_Mass) }
        };

        m_ShaderProgram  = Ref<Shader>(Shader::Create("Assets/Shaders/ComputeTextureQuad.glsl"));
        m_ComputeProgram = CreateComputeProgram("Assets/Shaders/Geodesic.glsl");
        
        m_CameraUBO = UniformBuffer::Create(128, 1);
        m_DiskUBO   = UniformBuffer::Create(sizeof(float) * 4, 2);
        
        uint32_t objUBOSize = sizeof(int) + 3 * sizeof(float)
            + 16 * (sizeof(glm::vec4) + sizeof(glm::vec4))
            + 16 * sizeof(float);
        m_ObjectsUBO = UniformBuffer::Create(objUBOSize, 3);

        auto result = QuadVAO();
        m_QuadVAO = result.first;
        m_Texture = result.second;
    }

    void Engine::UpdateWindowDimensions()
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        int oldWidth = m_Width;
        int oldHeight = m_Height;
        int oldComputeHeight = m_ComputeHeight;
        
        glfwGetFramebufferSize(window, &m_Width, &m_Height);
        
        if (oldWidth         != m_Width  || 
            oldHeight        != m_Height || 
            oldComputeHeight != m_ComputeHeight)
            UpdateComputeDimensions();
    }

    void Engine::UpdatePerformance(float deltaTime)
    {
        if (deltaTime > 0.0f)
            m_CurrentFPS = 1.0f / deltaTime;
    }

    void Engine::UpdateComputeDimensions()
    {
        m_Texture = Texture2D::Create(GetComputeWidth(), m_ComputeHeight);
    }

    void Engine::DrawFullScreenQuad()
    {
        RenderCommand::SetViewport(0, 0, m_Width, m_Height);
        
        m_ShaderProgram->Bind();
        m_QuadVAO->Bind();

        m_Texture->Bind(0);
        m_ShaderProgram->SetInt("u_ScreenTexture", 0);

        RenderCommand::DisableDepthTest();
        RenderCommand::DrawArrays(6);
        RenderCommand::EnableDepthTest();
    }

    void Engine::DispatchCompute(const Camera& cam)
    {
        int cw = GetComputeWidth();
        int ch = m_ComputeHeight;

        m_Texture->SetData(nullptr, cw * ch * 4);

        m_ComputeProgram->Bind();
        UploadCameraUBO(cam);
        UploadDiskUBO();
        UploadObjectsUBO(m_Objects);
        m_Texture->BindAsImage(0, false);

        uint32_t groupsX = static_cast<uint32_t>(std::ceil(cw / 16.0f));
        uint32_t groupsY = static_cast<uint32_t>(std::ceil(ch / 16.0f));
        m_ComputeProgram->Dispatch(groupsX, groupsY, 1);
        m_ComputeProgram->MemoryBarrier(IMAGE_ACCESS_BARRIER_BIT);
    }

    void Engine::UploadCameraUBO(const Camera& cam)
    {
        struct UBOData
        {
            glm::vec3 pos;     float _pad0;
            glm::vec3 right;   float _pad1;
            glm::vec3 up;      float _pad2;
            glm::vec3 forward; float _pad3;
            float tanHalfFov;
            float aspect;
            bool  moving;
            int   _pad4;
        } data;

        glm::vec3 fwd   = glm::normalize(cam.GetOrbitalTarget() - cam.GetOrbitalPosition());
        glm::vec3 up    = glm::vec3(0, 1, 0);
        glm::vec3 right = glm::normalize(glm::cross(fwd, up));
        up = glm::cross(right, fwd);

        data.pos        = cam.GetOrbitalPosition();
        data.right      = right;
        data.up         = up;
        data.forward    = fwd;
        data.tanHalfFov = static_cast<float>(tan(glm::radians(60.0f * 0.5f)));
        data.aspect     = static_cast<float>(GetComputeWidth()) / static_cast<float>(m_ComputeHeight);
        data.moving     = cam.IsDragging() || cam.IsPanning();

        m_CameraUBO->SetData(&data, sizeof(UBOData));
        m_CameraUBO->Bind(1);
    }

    void Engine::UploadObjectsUBO(const std::vector<ObjectData>& objs)
    {
        struct UBOData
        {
            int numObjects;
            float _pad0, _pad1, _pad2;
            glm::vec4 posRadius[16];
            glm::vec4 color[16];
            float mass[16];
        } data;

        size_t count = std::min(objs.size(), size_t(16));
        data.numObjects = static_cast<int>(count);

        for (size_t i = 0; i < count; ++i)
        {
            data.posRadius[i] = objs[i].m_PosRadius;
            data.color[i] = objs[i].m_Color;
            data.mass[i] = objs[i].m_Mass;
        }

        m_ObjectsUBO->SetData(&data, sizeof(data));
        m_ObjectsUBO->Bind(3);
    }

    void Engine::UploadDiskUBO()
    {
        float r1 = static_cast<float>(m_SagA.m_Rs * 2.2);
        float r2 = static_cast<float>(m_SagA.m_Rs * 5.2);
        float num = 2.0f;
        float thickness = 1e9f;
        float diskData[4] = { r1, r2, num, thickness };

        m_DiskUBO->SetData(diskData, sizeof(diskData));
        m_DiskUBO->Bind(2);
    }

    void Engine::UpdatePhysics(float deltaTime)
    {
        for (auto& obj : m_Objects)
        {
            for (auto& obj2 : m_Objects)
            {
                if (&obj == &obj2) continue;
                float dx = obj2.m_PosRadius.x - obj.m_PosRadius.x;
                float dy = obj2.m_PosRadius.y - obj.m_PosRadius.y;
                float dz = obj2.m_PosRadius.z - obj.m_PosRadius.z;
                float distance = sqrt(dx * dx + dy * dy + dz * dz);
                if (distance > 0)
                {
                    std::vector<double> direction = {dx / distance, dy / distance, dz / distance};
                    double Gforce = (G * obj.m_Mass * obj2.m_Mass) / (distance * distance);
                    double acc1 = Gforce / obj.m_Mass;
                    std::vector<double> acc = {direction[0] * acc1, direction[1] * acc1, direction[2] * acc1};
                    
                    if (m_Gravity)
                    {
                        obj.m_Velocity.x += static_cast<float>(acc[0]);
                        obj.m_Velocity.y += static_cast<float>(acc[1]);
                        obj.m_Velocity.z += static_cast<float>(acc[2]);

                        obj.m_PosRadius.x += static_cast<float>(obj.m_Velocity.x);
                        obj.m_PosRadius.y += static_cast<float>(obj.m_Velocity.y);
                        obj.m_PosRadius.z += static_cast<float>(obj.m_Velocity.z);
                    }
                }
            }
        }
    }

    void Engine::RenderScene()
    {
        RenderCommand::Clear();
        m_ShaderProgram->Bind();
        m_QuadVAO->Bind();
        m_Texture->Bind(0);
        RenderCommand::DrawArrays(6);
    }

    Ref<Shader> Engine::CreateComputeProgram(const char* path)
    {
        std::ifstream in(path);
        if(!in.is_open())
        {
            std::cerr << "Failed to open compute shader: " << path << "\n";
            return nullptr;
        }
        std::stringstream ss;
        ss << in.rdbuf();
        std::string srcStr = ss.str();
        return Ref<Shader>(Shader::CreateCompute("ComputeShader", srcStr));
    }

    std::pair<Ref<VertexArray>, Ref<Texture2D>> Engine::QuadVAO()
    {
        float quadVertices[] = 
        {
            // Positions   // TexCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        auto vertexBuffer = Ref<VertexBuffer>(VertexBuffer::Create(quadVertices, static_cast<uint32_t>(sizeof(quadVertices))));
        VertexBufferLayout layout;
        layout.Push<float>(2); // Position (x, y)
        layout.Push<float>(2); // TexCoord (u, v)
        vertexBuffer->SetLayout(layout);

        auto vertexArray = Ref<VertexArray>(VertexArray::Create());
        vertexArray->AddVertexBuffer(vertexBuffer);
        auto texture = Texture2D::Create(GetComputeWidth(), m_ComputeHeight);
        
        return { vertexArray, texture };
    }
}

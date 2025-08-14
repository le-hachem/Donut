#include "Engine.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"

namespace Donut
{
    Engine::Engine() 
        : m_SagA(glm::vec3(0.0f, 0.0f, 0.0f), 
          static_cast<float>(8.54e36)) 
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        glfwGetFramebufferSize(window, &m_Width, &m_Height);
        m_ComputeHeight = 420;
        m_ComputeWidth = (m_Width * 420) / m_Height;
        
        m_Camera.SetOrbitalMode(true);
        m_Camera.SetRadius(6.34194e10);
        m_Camera.SetMinRadius(1e10);
        m_Camera.SetMaxRadius(1e12);
        m_Camera.SetOrbitSpeed(0.01f);
        m_Camera.SetZoomSpeed(25e9f);
        
        m_Objects = 
        {
            { glm::vec4(4e11f, 0.0f, 0.0f, 4e10f), glm::vec4(1,1,0,1), static_cast<float>(1.98892e30) },
            { glm::vec4(0.0f, 0.0f, 4e11f, 4e10f), glm::vec4(1,0,0,1), static_cast<float>(1.98892e30) },
            { glm::vec4(0.0f, 0.0f, 0.0f, m_SagA.m_Rs), glm::vec4(0,0,0,1), static_cast<float>(m_SagA.m_Mass) }
        };

        m_QuadShader    = Shader::Create("Assets/Shaders/Quad.glsl");
        m_GridShader    = Shader::Create("Assets/Shaders/Grid.glsl");
        m_ComputeShader = Shader::CreateCompute("Geodesic", LoadComputeShader("Assets/Shaders/Geodesic.glsl"));
        
        m_CameraUBO = UniformBuffer::Create(128, 1);
        m_DiskUBO = UniformBuffer::Create(sizeof(float) * 4, 2);
        
        uint32_t objUBOSize = sizeof(int) + 3 * sizeof(float)
            + 16 * (sizeof(glm::vec4) + sizeof(glm::vec4))
            + 16 * sizeof(float);
        m_ObjectsUBO = UniformBuffer::Create(objUBOSize, 3);

        float quadVertices[] = 
        {
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
            
            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        m_QuadVAO = VertexArray::Create();
        
        auto vertexBuffer = VertexBuffer::Create(quadVertices, static_cast<uint32_t>(sizeof(quadVertices)));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        vertexBuffer->SetLayout(layout);
        m_QuadVAO->AddVertexBuffer(vertexBuffer);

        m_Texture = Texture2D::Create(m_Width, m_Height);
    }

    void Engine::GenerateGrid(const std::vector<ObjectData>& objects) 
    {
        const int gridSize = 25;
        const float spacing = 1e10f;

        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;

        for (int z = 0; z <= gridSize; ++z) 
        {
            for (int x = 0; x <= gridSize; ++x) 
            {
                float worldX = (x - gridSize / 2) * spacing;
                float worldZ = (z - gridSize / 2) * spacing;
                float y = 0.0f;

                for (const auto& obj : objects) 
                {
                    glm::vec3 objPos = glm::vec3(obj.m_PosRadius);
                    double mass = obj.m_Mass;
                    double radius = obj.m_PosRadius.w;

                    double r_s = 2.0 * G * mass / (c * c);
                    double dx = worldX - objPos.x;
                    double dz = worldZ - objPos.z;
                    double dist = sqrt(dx * dx + dz * dz);

                    if (dist > r_s) 
                    {
                        double deltaY = 2.0 * sqrt(r_s * (dist - r_s));
                        y += static_cast<float>(deltaY) - 3e10f;
                    } 
                    else 
                    {
                        y += 2.0f * static_cast<float>(sqrt(r_s * r_s)) - 3e10f;
                    }
                }

                vertices.emplace_back(worldX, y, worldZ);
            }
        }

        for (int z = 0; z < gridSize; ++z) 
        {
            for (int x = 0; x < gridSize; ++x) 
            {
                int i = z * (gridSize + 1) + x;
                indices.push_back(i);
                indices.push_back(i + 1);
                indices.push_back(i);
                indices.push_back(i + gridSize + 1);
            }
        }

        if (!m_GridVAO)
        {
            m_GridVAO = Ref<VertexArray>(VertexArray::Create());
            
            auto vertexBuffer = Ref<VertexBuffer>(VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(glm::vec3))));
            VertexBufferLayout layout;
            layout.Push<float>(3);
            vertexBuffer->SetLayout(layout);
            m_GridVAO->AddVertexBuffer(vertexBuffer);

            auto indexBuffer = Ref<IndexBuffer>(IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size())));
            m_GridVAO->SetIndexBuffer(indexBuffer);
        }
        else
        {
            auto vertexBuffer = m_GridVAO->GetVertexBuffers()[0];
            vertexBuffer->SetData(vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(glm::vec3)));
        }

        m_GridIndexCount = static_cast<int>(indices.size());
    }

    void Engine::DrawGrid(const glm::mat4& viewProj) 
    {
        if (!m_GridShader || !m_GridVAO) 
            return;
        
        m_GridShader->Bind();
        m_GridShader->SetMat4("viewProj", viewProj);
        
        RenderCommand::DisableDepthTest();
        RenderCommand::EnableBlending();
        
        m_GridVAO->Bind();
        glDrawElements(GL_LINES, m_GridIndexCount, GL_UNSIGNED_INT, nullptr);
        
        RenderCommand::EnableDepthTest();
    }

    void Engine::UpdateWindowDimensions() 
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        int oldWidth = m_Width;
        int oldHeight = m_Height;
        glfwGetFramebufferSize(window, &m_Width, &m_Height);
        m_ComputeHeight = 420;
        m_ComputeWidth = (m_Width * 420) / m_Height;
        
        if (oldWidth != m_Width || oldHeight != m_Height)
            m_Texture = Texture2D::Create(m_Width, m_Height);
    }

    void Engine::DrawFullScreenQuad() 
    {
        RenderCommand::SetViewport(0, 0, m_Width, m_Height);
        
        m_QuadShader->Bind();
        std::cout << "Texture ID: " << m_Texture->GetRendererID() << ", Size: " << m_Texture->GetWidth() << "x" << m_Texture->GetHeight() << std::endl;
        m_Texture->Bind(0);
        m_QuadShader->SetInt("screenTexture", 0);
        std::cout << "Set screenTexture uniform to slot 0\n";

        RenderCommand::DisableDepthTest();
        RenderCommand::DrawIndexed(m_QuadVAO, 6);
        RenderCommand::EnableDepthTest();
    }

    void Engine::DispatchCompute(const Camera& cam) 
    {
        int cw = m_ComputeWidth;
        int ch = m_ComputeHeight;

        if (m_Texture->GetWidth() != cw || m_Texture->GetHeight() != ch) 
            m_Texture = Texture2D::Create(cw, ch);

        m_ComputeShader->Bind();
        UploadCameraUBO(cam);
        UploadDiskUBO();
        UploadObjectsUBO(m_Objects);

        m_Texture->BindAsImage(0, false);

        uint32_t groupsX = (uint32_t)std::ceil(cw / 16.0f);
        uint32_t groupsY = (uint32_t)std::ceil(ch / 16.0f);
        m_ComputeShader->Dispatch(groupsX, groupsY, 1);
        m_ComputeShader->MemoryBarrier(IMAGE_ACCESS_BARRIER_BIT);
        
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
    }

    void Engine::UploadCameraUBO(const Camera& cam) 
    {
        struct UBOData 
        {
            glm::vec3 pos; float _pad0;
            glm::vec3 right; float _pad1;
            glm::vec3 up; float _pad2;
            glm::vec3 forward; float _pad3;
            float tanHalfFov;
            float aspect;
            bool moving;
            int _pad4;
        } data;

        glm::vec3 fwd = glm::normalize(cam.GetTarget() - cam.GetOrbitalPosition());
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = glm::normalize(glm::cross(fwd, up));
        up = glm::cross(right, fwd);

        data.pos = cam.GetOrbitalPosition();
        data.right = right;
        data.up = up;
        data.forward = fwd;
        data.tanHalfFov = static_cast<float>(tan(glm::radians(60.0f * 0.5f)));
        data.aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
        data.moving = cam.IsDragging() || cam.IsPanning();

        m_CameraUBO->SetData(&data, sizeof(UBOData));
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
    }

    void Engine::UploadDiskUBO() 
    {
        float r1 = static_cast<float>(m_SagA.m_Rs * 2.2);
        float r2 = static_cast<float>(m_SagA.m_Rs * 5.2);
        float num = 2.0f;
        float thickness = 1e9f;
        float diskData[4] = { r1, r2, num, thickness };

        m_DiskUBO->SetData(diskData, sizeof(diskData));
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
        glm::mat4 view = glm::lookAt(m_Camera.GetOrbitalPosition(), m_Camera.GetTarget(), glm::vec3(0,1,0));
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(m_Width)/m_Height, 1e9f, 1e14f);
        glm::mat4 viewProj = proj * view;
        
        GenerateGrid(m_Objects);
        DrawGrid(viewProj);
        
        RenderCommand::SetViewport(0, 0, m_Width, m_Height);
        DispatchCompute(m_Camera);
        DrawFullScreenQuad();
    }

    std::string Engine::LoadComputeShader(const std::string& path)
    {
        std::ifstream in(path);
        if(!in.is_open()) 
        {
            std::cerr << "Failed to open compute shader: " << path << "\n";
            return "";
        }

        std::stringstream ss;
        ss << in.rdbuf();
        std::string result = ss.str();
        if (result.empty()) 
            std::cerr << "Warning: Compute shader file is empty: " << path << "\n";
        else 
            std::cout << "Successfully loaded compute shader: " << path << " (size: " << result.size() << " bytes)\n";
        return result;
    }
}

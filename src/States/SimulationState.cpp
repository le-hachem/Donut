#include "SimulationState.h"
#include "Rendering/Renderer.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

namespace Donut
{
    void SimulationState::OnEnter()
    {
        m_Camera = CreateScope<Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
        m_Camera->SetPosition({ 0.0f, 0.0f, 3.0f });
        m_Camera->SetMouseSensitivity(0.1f);
        m_Camera->SetMovementSpeed(5.0f);

        float vertices[] = 
        {
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f
        };

        uint32_t indices[] = 
        {
            0, 1, 2,  2, 3, 0,
            1, 5, 6,  6, 2, 1,
            5, 4, 7,  7, 6, 5,
            4, 0, 3,  3, 7, 4,
            3, 2, 6,  6, 7, 3,
            4, 5, 1,  1, 0, 4
        };

        m_VertexArray = Ref<VertexArray>(VertexArray::Create());
        m_VertexBuffer = Ref<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
        
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(4);
        layout.Push<float>(2);
        m_VertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer = Ref<IndexBuffer>(IndexBuffer::Create(indices, 36));
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_Shader = Ref<Shader>(Shader::Create("Assets/Textured.glsl"));
        m_Texture = Texture2D::Create(256, 256);
        
        uint32_t* pixelData = new uint32_t[256 * 256];
        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                float u = (float)x / 256.0f;
                float v = (float)y / 256.0f;
                
                float wave = sin(u * 10.0f) * cos(v * 10.0f);
                wave = (wave + 1.0f) * 0.5f;
                
                uint8_t r = (uint8_t)(wave * 255);
                uint8_t g = (uint8_t)((1.0f - wave) * 255);
                uint8_t b = (uint8_t)((u + v) * 0.5f * 255);
                uint8_t a = 255;
                
                pixelData[y * 256 + x] = (a << 24) | (b << 16) | (g << 8) | r;
            }
        }
        
        m_Texture->SetData(pixelData, 256 * 256 * 4);
        delete[] pixelData;

        m_ComputeShader = Ref<Shader>(Shader::Create("Assets/TextureProcessor.glsl"));
        
        if (!m_ComputeShader)
        {
            DONUT_WARN("Failed to create compute shader! Falling back to CPU texture generation.");
            m_UseComputeShader = false;
        }
        else
            DONUT_INFO("Compute shader created successfully!");
        
        m_ProcessedTexture = Texture2D::Create(256, 256);
    }
    
    void SimulationState::OnExit()
    {
    }
    
    void SimulationState::OnUpdate(float deltaTime)
    {
        m_DeltaTime = deltaTime;
        HandleKeyInput(deltaTime);
        UpdateTexture();
    }
    
    void SimulationState::OnRender()
    {
        Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        Renderer::Clear();

        if (m_UseComputeShader && m_ProcessedTexture)
        {
            m_ProcessedTexture->Bind(0);
            m_Shader->SetInt("u_Texture", 0);
        }
        else if (m_Texture)
        {
            m_Texture->Bind(0);
            m_Shader->SetInt("u_Texture", 0);
        }

        glm::mat4 viewProjection = m_Camera ? m_Camera->GetViewProjectionMatrix() : glm::mat4(1.0f);
        
        glm::vec3 centerPos(0.0f, 0.0f, 0.0f);
        glm::vec3 rightPos(2.0f, 0.0f, 0.0f);
        glm::vec3 leftPos(-2.0f, 0.0f, 0.0f);
        glm::vec3 upPos(0.0f, 2.0f, 0.0f);
        glm::vec3 downPos(0.0f, -2.0f, 0.0f);
        glm::vec3 forwardPos(0.0f, 0.0f, 2.0f);
        glm::vec3 backPos(0.0f, 0.0f, -2.0f);
        
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), centerPos);
        Renderer::Submit(m_Shader, m_VertexArray, transform, viewProjection);
        transform = glm::translate(glm::mat4(1.0f), rightPos);
        Renderer::Submit(m_Shader, m_VertexArray, transform, viewProjection);
        transform = glm::translate(glm::mat4(1.0f), leftPos);
        Renderer::Submit(m_Shader, m_VertexArray, transform, viewProjection);
        transform = glm::translate(glm::mat4(1.0f), upPos);
        Renderer::Submit(m_Shader, m_VertexArray, transform, viewProjection);
        transform = glm::translate(glm::mat4(1.0f), downPos);
        Renderer::Submit(m_Shader, m_VertexArray, transform, viewProjection);
        transform = glm::translate(glm::mat4(1.0f), forwardPos);
        Renderer::Submit(m_Shader, m_VertexArray, transform, viewProjection);
        transform = glm::translate(glm::mat4(1.0f), backPos);
        Renderer::Submit(m_Shader, m_VertexArray, transform, viewProjection);
    }
    
    void SimulationState::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) 
        {
            if (m_Camera)
                m_Camera->SetProjection(45.0f, (float)e.GetWidth() / (float)e.GetHeight(), 0.1f, 100.0f);
            return true;
        });
        
        dispatcher.Dispatch<WindowFocusEvent>([this](WindowFocusEvent& e) 
        {
            m_FirstMouse = true;
            m_MouseDragging = false;
            return true;
        });
        
        dispatcher.Dispatch<WindowLostFocusEvent>([this](WindowLostFocusEvent& e) 
        {
            m_MouseDragging = false;
            return true;
        });
        
        dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) 
        {
            m_Keys[e.GetKeyCode()] = true;
            return true;
        });
        
        dispatcher.Dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& e) 
        {
            m_Keys[e.GetKeyCode()] = false;
            return true;
        });
        
        dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& e) 
        {
            if (!m_MouseDragging)
                return true;
                
            if (m_FirstMouse)
            {
                m_LastX = e.GetX();
                m_LastY = e.GetY();
                m_FirstMouse = false;
            }

            float xOffset = m_LastX - e.GetX();
            float yOffset = e.GetY() - m_LastY;

            m_LastX = e.GetX();
            m_LastY = e.GetY();

            if (m_Camera)
                m_Camera->OnMouseMove(xOffset, yOffset);
            
            return true;
        });
        
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) 
        {
            if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
            {
                m_MouseDragging = true;
                m_FirstMouse = true;
            }
            return true;
        });
        
        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e) 
        {
            if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
                m_MouseDragging = false;
            return true;
        });
    }
    
    void SimulationState::HandleKeyInput(float deltaTime)
    {
        if (m_Camera)
        {
            if (m_Keys[GLFW_KEY_W])
                m_Camera->MoveForward(deltaTime);
            if (m_Keys[GLFW_KEY_S])
                m_Camera->MoveBackward(deltaTime);
            if (m_Keys[GLFW_KEY_A])
                m_Camera->MoveLeft(deltaTime);
            if (m_Keys[GLFW_KEY_D])
                m_Camera->MoveRight(deltaTime);
            if (m_Keys[GLFW_KEY_SPACE]) 
                m_Camera->MoveUp(deltaTime);
            if (m_Keys[GLFW_KEY_LEFT_SHIFT])
                m_Camera->MoveDown(deltaTime);    
        }
        
        static bool cKeyPressed = false;
        if (m_Keys[GLFW_KEY_C] && !cKeyPressed)
        {
            m_UseComputeShader = !m_UseComputeShader;
            DONUT_INFO("Compute shader: {}", (m_UseComputeShader ? "ON" : "OFF"));
            cKeyPressed = true;
        }
        if (!m_Keys[GLFW_KEY_C])
            cKeyPressed = false;
        if (m_Keys[GLFW_KEY_UP])
            m_ComputeBrightness += deltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_DOWN])
            m_ComputeBrightness -= deltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_LEFT])
            m_ComputeContrast -= deltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_RIGHT])
            m_ComputeContrast += deltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_Q])
            m_ComputeSaturation -= deltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_E])
            m_ComputeSaturation += deltaTime * 0.5f;
        
        m_ComputeBrightness = glm::clamp(m_ComputeBrightness, -1.0f, 1.0f);
        m_ComputeContrast = glm::clamp(m_ComputeContrast, 0.1f, 3.0f);
        m_ComputeSaturation = glm::clamp(m_ComputeSaturation, 0.0f, 2.0f);
        
        static float statusTimer = 0.0f;
        statusTimer += deltaTime;
        if (statusTimer > 2.0f)
        {
            DONUT_INFO("Compute Shader Status:\nBrightness: {}\nContrast: {}\nSaturation: {}",
                       m_ComputeBrightness, m_ComputeContrast, m_ComputeSaturation);
            statusTimer = 0.0f;
        }
    }
    
    void SimulationState::UpdateTexture()
    {
        if (!m_Texture)
            return;
            
        m_TextureTime += m_DeltaTime;
        
        if (m_UseComputeShader && m_ComputeShader && m_ProcessedTexture)
        {
            m_ComputeShader->Bind();
            m_Texture->Bind(0);
            m_ComputeShader->SetInt("u_InputTexture", 0);
            m_ProcessedTexture->BindAsImage(1, false);
            m_ComputeShader->SetFloat2("u_TextureSize", glm::vec2(256.0f, 256.0f));
            m_ComputeShader->SetFloat("u_Time", m_TextureTime);
            m_ComputeShader->SetFloat("u_Brightness", m_ComputeBrightness);
            m_ComputeShader->SetFloat("u_Contrast", m_ComputeContrast);
            m_ComputeShader->SetFloat("u_Saturation", m_ComputeSaturation);
            m_ComputeShader->Dispatch(16, 16, 1);
            m_ComputeShader->MemoryBarrier(IMAGE_ACCESS_BARRIER_BIT);
        }
        else
        {
            uint32_t* pixelData = new uint32_t[256 * 256];
            for (int y = 0; y < 256; y++)
            {
                for (int x = 0; x < 256; x++)
                {
                    float time = m_TextureTime * 2.0f;
                    float u = (float)x / 256.0f;
                    float v = (float)y / 256.0f;
                    
                    float wave = sin(u * 10.0f + time) * cos(v * 10.0f + time * 0.5f);
                    wave = (wave + 1.0f) * 0.5f;
                    
                    uint8_t r = (uint8_t)(wave * 255);
                    uint8_t g = (uint8_t)((1.0f - wave) * 255);
                    uint8_t b = (uint8_t)((u + v) * 0.5f * 255);
                    uint8_t a = 255;
                    
                    pixelData[y * 256 + x] = (a << 24) | (b << 16) | (g << 8) | r;
                }
            }
            
            m_Texture->SetData(pixelData, 256 * 256 * 4);
            delete[] pixelData;
        }
    }
};

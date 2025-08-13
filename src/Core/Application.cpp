#include "Application.h"

#include "Rendering/Renderer.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

namespace Donut
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name, int width, int height)
        : m_Running(true), m_Minimized(false)
    {
        s_Instance = this;

        m_Window = std::make_unique<Window>(name, width, height);
        m_Window->SetEventCallback([this](Event& event) 
        {
            OnEvent(event);
        });

        OnInit();
    }

    Application::~Application()
    {
        OnShutdown();
        s_Instance = nullptr;
    }

    void Application::Run()
    {
        while (m_Running)
        {
            if (!m_Minimized)
            {
                OnUpdate();
                OnRender();
            }

            m_Window->OnUpdate();
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>([this, &event](WindowCloseEvent& e) 
        {
            m_Running = false;
            event.Handled = true;
            return true;
        });
        
        dispatcher.Dispatch<WindowResizeEvent>([this, &event](WindowResizeEvent& e) 
        {
            if (e.GetWidth() == 0 || e.GetHeight() == 0)
                m_Minimized = true;
            else
                m_Minimized = false;
            
            Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
            
            if (m_Camera)
                m_Camera->SetProjection(45.0f, (float)e.GetWidth() / (float)e.GetHeight(), 0.1f, 100.0f);
            
            event.Handled = true;
            return true;
        });
        
        dispatcher.Dispatch<WindowFocusEvent>([this, &event](WindowFocusEvent& e) 
        {
            m_FirstMouse = true;
            m_MouseDragging = false;
            event.Handled = true;
            return true;
        });
        
        dispatcher.Dispatch<WindowLostFocusEvent>([this, &event](WindowLostFocusEvent& e) 
        {
            m_MouseDragging = false;
            event.Handled = true;
            return true;
        });
        
        dispatcher.Dispatch<KeyPressedEvent>([this, &event](KeyPressedEvent& e) 
        {
            m_Keys[e.GetKeyCode()] = true;
            return true;
        });
        
        dispatcher.Dispatch<KeyReleasedEvent>([this, &event](KeyReleasedEvent& e) 
        {
            m_Keys[e.GetKeyCode()] = false;
            return true;
        });
        
        dispatcher.Dispatch<MouseMovedEvent>([this, &event](MouseMovedEvent& e) 
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
        
        dispatcher.Dispatch<MouseButtonPressedEvent>([this, &event](MouseButtonPressedEvent& e) 
        {
            if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
            {
                m_MouseDragging = true;
                m_FirstMouse = true;
            }
            return true;
        });
        
        dispatcher.Dispatch<MouseButtonReleasedEvent>([this, &event](MouseButtonReleasedEvent& e) 
        {
            if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
                m_MouseDragging = false;
            return true;
        });
    }

    void Application::OnInit()     
    { 
        Renderer::Init();
        Renderer::OnWindowResize(1280, 720);
        RenderCommand::SetFaceCulling(false);

        m_Camera = std::make_unique<Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
        m_Camera->SetPosition({ 0.0f, 0.0f, 3.0f });
        m_Camera->SetMouseSensitivity(0.1f);
        m_Camera->SetMovementSpeed(5.0f);

        float vertices[] = 
        {
            // Position (3) | Color (4) | TexCoords (2)
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

        m_VertexArray = std::shared_ptr<VertexArray>(VertexArray::Create());
        m_VertexBuffer = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
        
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(4);
        layout.Push<float>(2);
        m_VertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(indices, 36));
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_Shader = std::shared_ptr<Shader>(Shader::Create("assets/Textured.glsl"));

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

        m_ComputeShader = std::shared_ptr<Shader>(Shader::Create("assets/TextureProcessor.glsl"));
        
        if (!m_ComputeShader)
        {
            std::cout << "Failed to create compute shader! Falling back to CPU texture generation." << std::endl;
            m_UseComputeShader = false;
        }
        else
            std::cout << "Compute shader created successfully!" << std::endl;
        
        m_ProcessedTexture = Texture2D::Create(256, 256);
        m_ComputeBrightness = 0.0f;
        m_ComputeContrast = 1.0f;
        m_ComputeSaturation = 1.0f;
        m_UseComputeShader = true;
        
        std::cout << "=== Compute Shader Test ===" << std::endl;
        std::cout << "C - Toggle compute shader on/off" << std::endl;
        std::cout << "Arrow Keys - Adjust brightness (Up/Down) and contrast (Left/Right)" << std::endl;
        std::cout << "Q/E - Adjust saturation" << std::endl;
        std::cout << "WASD - Move camera" << std::endl;
        std::cout << "Mouse - Look around" << std::endl;
        std::cout << "==========================" << std::endl;
    }

    void Application::OnShutdown() 
    { 
        Renderer::Shutdown();
    }

    void Application::OnUpdate()
    {
        float currentFrame = (float)glfwGetTime();
        m_DeltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        if (m_Camera)
        {
            if (m_Keys[GLFW_KEY_W])
                m_Camera->MoveForward(m_DeltaTime);
            if (m_Keys[GLFW_KEY_S])
                m_Camera->MoveBackward(m_DeltaTime);
            if (m_Keys[GLFW_KEY_A])
                m_Camera->MoveLeft(m_DeltaTime);
            if (m_Keys[GLFW_KEY_D])
                m_Camera->MoveRight(m_DeltaTime);
            if (m_Keys[GLFW_KEY_SPACE]) 
                m_Camera->MoveUp(m_DeltaTime);
            if (m_Keys[GLFW_KEY_LEFT_SHIFT])
                m_Camera->MoveDown(m_DeltaTime);    
        }
        
        static bool cKeyPressed = false;
        if (m_Keys[GLFW_KEY_C] && !cKeyPressed)
        {
            m_UseComputeShader = !m_UseComputeShader;
            std::cout << "Compute shader: " << (m_UseComputeShader ? "ON" : "OFF") << std::endl;
            cKeyPressed = true;
        }
        if (!m_Keys[GLFW_KEY_C])
            cKeyPressed = false;
        if (m_Keys[GLFW_KEY_UP])
            m_ComputeBrightness += m_DeltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_DOWN])
            m_ComputeBrightness -= m_DeltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_LEFT])
            m_ComputeContrast -= m_DeltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_RIGHT])
            m_ComputeContrast += m_DeltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_Q])
            m_ComputeSaturation -= m_DeltaTime * 0.5f;
        if (m_Keys[GLFW_KEY_E])
            m_ComputeSaturation += m_DeltaTime * 0.5f;
        
        m_ComputeBrightness = glm::clamp(m_ComputeBrightness, -1.0f, 1.0f);
        m_ComputeContrast = glm::clamp(m_ComputeContrast, 0.1f, 3.0f);
        m_ComputeSaturation = glm::clamp(m_ComputeSaturation, 0.0f, 2.0f);
        
        static float statusTimer = 0.0f;
        statusTimer += m_DeltaTime;
        if (statusTimer > 2.0f)
        {
            std::cout << "Compute Shader Status - Brightness: " << m_ComputeBrightness 
                      << ", Contrast: " << m_ComputeContrast 
                      << ", Saturation: " << m_ComputeSaturation << std::endl;
            statusTimer = 0.0f;
        }
        
        UpdateTexture();
    }

    void Application::OnRender()
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

    void Application::UpdateTexture()
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
}

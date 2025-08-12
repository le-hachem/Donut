#include "Application.h"

#include "Rendering/Renderer.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Shader.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
            
            event.Handled = true;
            return true;
        });
        
        dispatcher.Dispatch<KeyPressedEvent>([this, &event](KeyPressedEvent& e) 
        {
            if (e.GetKeyCode() == 256)
                Close();
            return true;
        });
    }

    void Application::OnInit()     
    { 
        Renderer::Init();

        float vertices[3 * 7] = 
        {
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
        };

        uint32_t indices[3] = { 0, 1, 2 };

        m_VertexArray = std::shared_ptr<VertexArray>(VertexArray::Create());
        m_VertexBuffer = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
        
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(4);
        m_VertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(indices, 3));
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_Shader = std::shared_ptr<Shader>(Shader::Create("assets/Basic.glsl"));
    }

    void Application::OnShutdown() 
    { 
        Renderer::Shutdown();
    }

    void Application::OnUpdate()
    {
    }

    void Application::OnRender()
    {
        Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Renderer::Clear();

        Renderer::BeginScene();

        glm::mat4 viewProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        
        Renderer::Submit(m_Shader, m_VertexArray, glm::mat4(1.0f));

        Renderer::EndScene();
    }
}

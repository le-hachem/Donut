#include "Application.h"
#include <iostream>

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

    void Application::OnInit()     { }
    void Application::OnShutdown() { }

    void Application::OnUpdate()
    {
    }

    void Application::OnRender()
    {
    }
}

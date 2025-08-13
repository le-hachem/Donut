#pragma once

#include <memory>
#include <functional>

#include "Window.h"
#include "Event.h"
#include "StateManager.h"
#include "Log.h"

namespace Donut
{
    class Application
    {
    public:
        Application(const std::string& name = "Donut", 
                    int width = 1280, int height = 720);
        ~Application();

        void Run();
        void Close();

        Window& GetWindow()       { return *m_Window;   }
        static Application& Get() { return *s_Instance; }
    private:
        void OnInit();
        void OnShutdown();
        void OnUpdate();
        void OnRender();
        void OnEvent(Event& event);
    private:
        std::unique_ptr<StateManager> m_StateManager;
        std::unique_ptr<Window> m_Window;

        bool m_Running;
        bool m_Minimized;
        
        float m_DeltaTime = 0.0f;
        float m_LastFrame = 0.0f;

        static Application* s_Instance;
    };
}
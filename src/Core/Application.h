#pragma once

#include "Memory.h"
#include "Window.h"
#include "Event.h"
#include "Log.h"
#include "StateManager.h"

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
        Scope<StateManager> m_StateManager;
        Scope<Window> m_Window;

        bool m_Running;
        bool m_Minimized;
        
        float m_DeltaTime = 0.0f;
        float m_LastFrame = 0.0f;

        static Application* s_Instance;
    };
}
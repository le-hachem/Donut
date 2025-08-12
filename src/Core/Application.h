#pragma once

#include "Window.h"
#include "Event.h"
#include <memory>
#include <functional>

namespace Donut
{
    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;
    class Shader;
};

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
        std::unique_ptr<Window> m_Window;
        bool m_Running;
        bool m_Minimized;

        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        std::shared_ptr<Shader> m_Shader;

        static Application* s_Instance;
    };
}
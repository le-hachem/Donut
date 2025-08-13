#pragma once

#include <memory>
#include <functional>

#include "Window.h"
#include "Event.h"
#include "Camera.h"
#include "Log.h"

#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

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
        void UpdateTexture();
    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running;
        bool m_Minimized;

        std::shared_ptr<VertexArray>  m_VertexArray;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer>  m_IndexBuffer;
        std::shared_ptr<Shader>       m_Shader;
        std::shared_ptr<Texture2D>    m_Texture;
        
        std::shared_ptr<Shader>    m_ComputeShader;
        std::shared_ptr<Texture2D> m_ProcessedTexture;
        float m_ComputeBrightness;
        float m_ComputeContrast;
        float m_ComputeSaturation;
        bool  m_UseComputeShader;
        
        std::unique_ptr<Camera> m_Camera;
        
        bool  m_Keys[1024]    = { false };
        bool  m_FirstMouse    = true;
        float m_LastX         = 0.0f;
        float m_LastY         = 0.0f;
        float m_DeltaTime     = 0.0f;
        float m_LastFrame     = 0.0f;
        bool  m_MouseDragging = false;
        float m_TextureTime   = 0.0f;

        static Application* s_Instance;
    };
}
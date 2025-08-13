#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <memory>

namespace Donut
{
    template<typename T>
    using Scope = std::unique_ptr<T>;
    
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    class RendererAPI 
    {
    public:
        enum class API 
        {
            None   = 0, 
            OpenGL = 1,
            Vulkan = 2,
        };
    public:
        virtual ~RendererAPI() = default;

        virtual void Init()                                       = 0;
        virtual void SetViewport(uint32_t x, uint32_t y, 
                                 uint32_t width, uint32_t height) = 0;
        virtual void SetClearColor(const glm::vec4& color)        = 0;
        virtual void Clear()                                      = 0;
        virtual void EnableDepthTest()                            = 0;
        virtual void SetFaceCulling(bool enabled)                 = 0;

        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, 
                                 uint32_t indexCount = 0)         = 0;

        inline static API GetAPI() { return s_API; }
        static Scope<RendererAPI> Create();
    private:
        static API s_API;
    };

    class RenderCommand 
    {
    public:
        inline static void Init() 
        {
            s_RendererAPI->Init();
        }

        inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            s_RendererAPI->SetViewport(x, y, width, height);
        }

        inline static void SetClearColor(const glm::vec4& color)
        {
            s_RendererAPI->SetClearColor(color);
        }

        inline static void Clear()
        {
            s_RendererAPI->Clear();
        }

        inline static void EnableDepthTest()
        {
            s_RendererAPI->EnableDepthTest();
        }

        inline static void SetFaceCulling(bool enabled)
        {
            s_RendererAPI->SetFaceCulling(enabled);
        }

        inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0)
        {
            s_RendererAPI->DrawIndexed(vertexArray, indexCount);
        }

    private:
        static Scope<RendererAPI> s_RendererAPI;
    };

    class Renderer 
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32_t width, uint32_t height);

        static void Submit(const std::shared_ptr<Shader>& shader, 
                           const std::shared_ptr<VertexArray>& vertexArray,
                           const glm::mat4& transform,
                           const glm::mat4& viewProjection);

        static void SetClearColor(const glm::vec4& color);
        static void Clear();

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    };
};

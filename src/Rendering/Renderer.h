#pragma once

#include "Core/Memory.h"
#include "VertexArray.h"
#include "Shader.h"

#include <glm/glm.hpp>

namespace Donut
{
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
        virtual void DisableDepthTest()                           = 0;
        virtual void SetFaceCulling(bool enabled)                 = 0;
        virtual void EnableBlending()                             = 0;
        virtual void DisableBlending()                            = 0;

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, 
                                 uint32_t indexCount = 0)         = 0;

        inline static API GetAPI()         { return s_API; }
        inline static void SetAPI(API api) { s_API = api;  }
        static Ref<RendererAPI> Create();
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

        inline static void DisableDepthTest()
        {
            s_RendererAPI->DisableDepthTest();
        }

        inline static void SetFaceCulling(bool enabled)
        {
            s_RendererAPI->SetFaceCulling(enabled);
        }

        inline static void EnableBlending()
        {
            s_RendererAPI->EnableBlending();
        }

        inline static void DisableBlending()
        {
            s_RendererAPI->DisableBlending();
        }

        inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
        {
            s_RendererAPI->DrawIndexed(vertexArray, indexCount);
        }

    private:
        static Ref<RendererAPI> s_RendererAPI;
    };

    class Renderer 
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32_t width, uint32_t height);

        static void Submit(const Ref<Shader>& shader, 
                           const Ref<VertexArray>& vertexArray,
                           const glm::mat4& transform,
                           const glm::mat4& viewProjection);

        static void SetClearColor(const glm::vec4& color);
        static void Clear();

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    };
};

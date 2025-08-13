#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>

namespace Donut 
{
    std::unique_ptr<RendererAPI> RendererAPI::Create()
     {
        switch (s_API)
        {
            case API::OpenGL:
                return std::make_unique<OpenGLRendererAPI>();
            case API::Vulkan:
                return std::make_unique<VulkanRendererAPI>();
            default:
                return nullptr;
        }
    }

    RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

    void Renderer::Init()
    {
        RenderCommand::Init();
        RenderCommand::EnableDepthTest();
    }

    void Renderer::Shutdown()
    {
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, 
                          const std::shared_ptr<VertexArray>& vertexArray,
                          const glm::mat4& transform,
                          const glm::mat4& viewProjection) 
    {
        shader->Bind();
        shader->SetMat4("u_ViewProjection", viewProjection);
        shader->SetMat4("u_Transform", transform);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

    std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

    void Renderer::SetClearColor(const glm::vec4& color)
    {
        RenderCommand::SetClearColor(color);
    }

    void Renderer::Clear()
    {
        RenderCommand::Clear();
    }
};

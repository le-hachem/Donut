#include "Renderer.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

namespace Donut {

    Scope<RendererAPI> RendererAPI::Create() {
        switch (s_API) {
            case API::OpenGL:
                return std::make_unique<OpenGLRendererAPI>();
            default:
                return nullptr;
        }
    }

    RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

    Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

    void Renderer::Init() {
        RenderCommand::Init();
    }

    void Renderer::Shutdown() {
        delete s_SceneData;
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer::BeginScene() {
        // Set up scene data
        s_SceneData->ViewProjectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    }

    void Renderer::EndScene() {
        // Clean up scene data
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, 
                         const std::shared_ptr<VertexArray>& vertexArray,
                         const glm::mat4& transform) {
        shader->Bind();
        shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        shader->SetMat4("u_Transform", transform);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

    Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

    void Renderer::SetClearColor(const glm::vec4& color) {
        RenderCommand::SetClearColor(color);
    }

    void Renderer::Clear() {
        RenderCommand::Clear();
    }

}

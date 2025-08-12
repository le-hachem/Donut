#include "Texture.h"
#include "Renderer.h"
#include "../Platform/OpenGL/OpenGLTexture.h"

namespace Donut
{
    std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLTexture2D>(width, height);
        case RendererAPI::API::None:
            return nullptr;
        default:
            return nullptr;
        }
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLTexture2D>(path);
        case RendererAPI::API::None:
            return nullptr;
        default:
            return nullptr;
        }
    }
};
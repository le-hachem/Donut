#include "Texture.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace Donut
{
    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLTexture2D>(width, height);
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanTexture2D>(width, height);
        case RendererAPI::API::None:
            return nullptr;
        default:
            return nullptr;
        }
    }

    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLTexture2D>(path);
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanTexture2D>(path);
        case RendererAPI::API::None:
            return nullptr;
        default:
            return nullptr;
        }
    }

    Ref<CubemapTexture> CubemapTexture::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLCubemapTexture>(width, height);
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanCubemapTexture>(width, height);
        case RendererAPI::API::None:
            return nullptr;
        default:
            return nullptr;
        }
    }

    Ref<CubemapTexture> CubemapTexture::CreateFromHDRI(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLCubemapTexture>(path);
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanCubemapTexture>(path);
        case RendererAPI::API::None:
            return nullptr;
        default:
            return nullptr;
        }
    }
};
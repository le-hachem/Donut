#include "UniformBuffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Donut
{
    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLUniformBuffer>(size, binding);
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanUniformBuffer>(size, binding);
        case RendererAPI::API::None:
            return nullptr;
        default:
            return nullptr;
        }
    }
};

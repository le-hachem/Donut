#include "VertexBuffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/Vulkan/VulkanVertexBuffer.h"

namespace Donut
{
    Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size)
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return CreateRef<OpenGLVertexBuffer>(data, size);
            case RendererAPI::API::Vulkan:
                return CreateRef<VulkanVertexBuffer>((float*)data, size);
            default:
                return nullptr;
        }
    }
};

#include "VertexBuffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/Vulkan/VulkanVertexBuffer.h"

namespace Donut
{
    VertexBuffer* VertexBuffer::Create(const void* data, uint32_t size)
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return new OpenGLVertexBuffer(data, size);
            case RendererAPI::API::Vulkan:
                return new VulkanVertexBuffer((float*)data, size);
            default:
                return nullptr;
        }
    }
};

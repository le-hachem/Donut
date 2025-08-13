#include "IndexBuffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/Vulkan/VulkanIndexBuffer.h"

namespace Donut 
{
    IndexBuffer* IndexBuffer::Create(const uint32_t* indices, uint32_t count) 
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return new OpenGLIndexBuffer(indices, count);
            case RendererAPI::API::Vulkan:
                return new VulkanIndexBuffer((uint32_t*)indices, count);
            default:
                return nullptr;
        }
    }
};

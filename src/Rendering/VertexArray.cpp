#include "VertexArray.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Donut
{
    uint32_t VertexBufferElement::GetSizeOfType(uint32_t type)
    {
        switch (type) 
        {
            case 0x1406: return 4; // GL_FLOAT
            case 0x1405: return 4; // GL_UNSIGNED_INT
            case 0x1401: return 1; // GL_UNSIGNED_BYTE
            default: return 0;
        }
    }

    Ref<VertexArray> VertexArray::Create() 
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return CreateRef<OpenGLVertexArray>();
            case RendererAPI::API::Vulkan:
                return CreateRef<VulkanVertexArray>();
            default:
                return nullptr;
        }
    }
};

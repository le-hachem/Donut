#include "VertexBuffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"

namespace Donut
{
    VertexBuffer* VertexBuffer::Create(const void* data, uint32_t size) 
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return new OpenGLVertexBuffer(data, size);
            default:
                return nullptr;
        }
    }
};

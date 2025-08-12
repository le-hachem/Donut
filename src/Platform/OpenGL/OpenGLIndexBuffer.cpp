#include "OpenGLIndexBuffer.h"
#include <glad/glad.h>

namespace Donut
{
    OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count)
        : m_Count(count)
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() 
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLIndexBuffer::Bind() const 
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLIndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};

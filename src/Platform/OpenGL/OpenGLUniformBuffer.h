#pragma once

#include "Rendering/UniformBuffer.h"
#include <glad/glad.h>

namespace Donut
{
    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint32_t size, uint32_t binding);
        virtual ~OpenGLUniformBuffer();

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
        virtual void Bind(uint32_t binding) override;

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_Size = 0;
        uint32_t m_Binding = 0;
    };
};

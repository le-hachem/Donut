#pragma once

#include "Rendering/UniformBuffer.h"

namespace Donut
{
    class VulkanUniformBuffer : public UniformBuffer
    {
    public:
        VulkanUniformBuffer(uint32_t size, uint32_t binding);
        virtual ~VulkanUniformBuffer();

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
        virtual void Bind(uint32_t binding)                                        override;
    private:
        uint32_t m_Size;
        uint32_t m_Binding;
    };
};

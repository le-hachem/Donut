#pragma once

#include "Core/Memory.h"
#include <cstdint>

namespace Donut
{
    class UniformBuffer
    {
    public:
        virtual ~UniformBuffer() = default;

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
        virtual void Bind(uint32_t binding) = 0;

        static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
    };
};

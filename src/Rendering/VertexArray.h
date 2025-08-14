#pragma once

#include "Core/Memory.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <vector>

namespace Donut
{
    class VertexArray 
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind()   const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)    = 0;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>& GetIndexBuffer()                 const = 0;

        static Ref<VertexArray> Create();
    };
};

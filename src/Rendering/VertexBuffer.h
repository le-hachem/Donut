#pragma once

#include <cstdint>
#include <vector>

namespace Donut 
{
    struct VertexBufferElement 
    {
        uint32_t type;
        uint32_t count;
        uint8_t normalized;
        uint32_t offset;

        static uint32_t GetSizeOfType(uint32_t type);
    };

    class VertexBufferLayout
    {
    public:
        VertexBufferLayout() = default;
        ~VertexBufferLayout() = default;

        template<typename T>
        void Push(uint32_t count) 
        {
            static_assert(false);
        }

        template<>
        void Push<float>(uint32_t count) 
        {
            m_Elements.push_back({ 0x1406, count, 0, m_Stride }); // GL_FLOAT
            m_Stride += count * VertexBufferElement::GetSizeOfType(0x1406);
        }

        template<>
        void Push<uint32_t>(uint32_t count) 
        {
            m_Elements.push_back({ 0x1405, count, 0, m_Stride }); // GL_UNSIGNED_INT
            m_Stride += count * VertexBufferElement::GetSizeOfType(0x1405);
        }

        template<>
        void Push<uint8_t>(uint32_t count) 
        {
            m_Elements.push_back({ 0x1401, count, 1, m_Stride }); // GL_UNSIGNED_BYTE
            m_Stride += count * VertexBufferElement::GetSizeOfType(0x1401);
        }

        inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
        inline uint32_t GetStride() const { return m_Stride; }

    private:
        std::vector<VertexBufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual const VertexBufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const VertexBufferLayout& layout) = 0;

        static VertexBuffer* Create(const void* data, uint32_t size);
    };
};

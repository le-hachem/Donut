#pragma once

#include "Rendering/VertexBuffer.h"

namespace Donut
{
	class VulkanVertexBuffer
        : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(uint32_t size);
		VulkanVertexBuffer(float* vertices, uint32_t size);
		virtual ~VulkanVertexBuffer();

		virtual void Bind()   const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const VertexBufferLayout& GetLayout()      const override { return m_Layout;  }
		virtual void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }
	private:
		uint32_t           m_RendererID;
		VertexBufferLayout m_Layout;
	};
};

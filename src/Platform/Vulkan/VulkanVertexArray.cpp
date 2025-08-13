#include "VulkanVertexArray.h"

namespace Donut
{
	VulkanVertexArray::VulkanVertexArray()
	{
		// TODO(Hachem): Implement Vulkan vertex array creation
	}

	VulkanVertexArray::~VulkanVertexArray()
	{
		// TODO(Hachem): Implement Vulkan vertex array cleanup
	}

	void VulkanVertexArray::Bind() const
	{
		// TODO(Hachem): Implement Vulkan vertex array binding
	}

	void VulkanVertexArray::Unbind() const
	{
		// TODO(Hachem): Implement Vulkan vertex array unbinding
	}

	void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		// TODO(Hachem): Implement Vulkan vertex buffer addition
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		// TODO(Hachem): Implement Vulkan index buffer setting
		m_IndexBuffer = indexBuffer;
	}
};

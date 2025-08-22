#include "VulkanTexture.h"

namespace Donut
{
	VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		m_InternalFormat = 0;
		m_DataFormat = 0;
		m_RendererID = 0;
	}

	VulkanTexture2D::VulkanTexture2D(const std::string& path)
		: m_Path(path)
	{
		m_Width = 1;
		m_Height = 1;
		m_InternalFormat = 0;
		m_DataFormat = 0;
		m_RendererID = 0;
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
	}

	void VulkanTexture2D::SetData(void* data, uint32_t size)
	{
	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{
	}

	void VulkanTexture2D::BindAsImage(uint32_t slot, bool readOnly) const
	{
	}

	// Vulkan Cubemap Implementation (Placeholder)
	VulkanCubemapTexture::VulkanCubemapTexture(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		m_InternalFormat = 0;
		m_DataFormat = 0;
		m_RendererID = 0;
	}

	VulkanCubemapTexture::VulkanCubemapTexture(const std::string& path)
		: m_Path(path)
	{
		m_Width = 1024;
		m_Height = 1024;
		m_InternalFormat = 0;
		m_DataFormat = 0;
		m_RendererID = 0;
	}

	VulkanCubemapTexture::~VulkanCubemapTexture()
	{
	}

	void VulkanCubemapTexture::SetData(void* data, uint32_t size)
	{
	}

	void VulkanCubemapTexture::Bind(uint32_t slot) const
	{
	}

	void VulkanCubemapTexture::BindAsImage(uint32_t slot, bool readOnly) const
	{
	}
};

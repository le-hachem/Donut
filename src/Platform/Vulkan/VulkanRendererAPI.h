#pragma once

#include "Rendering/Renderer.h"

namespace Donut
{
	class VulkanRendererAPI 
		: public RendererAPI
	{
	public:
		virtual void Init()                                       override;
		virtual void SetViewport(uint32_t x, uint32_t y,
								 uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color)        override;
		virtual void Clear()                                      override;
		virtual void EnableDepthTest()                            override;
		virtual void SetFaceCulling(bool enabled)                 override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, 
								 uint32_t indexCount = 0) override;
	};
};

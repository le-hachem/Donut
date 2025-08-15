#include "VulkanRendererAPI.h"

namespace Donut
{
    void VulkanRendererAPI::Init()
    {
        // TODO(Hachem): Implement Vulkan renderer API initialization
    }

    void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        // TODO(Hachem): Implement Vulkan viewport setting
    }

    void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
    {
        // TODO(Hachem): Implement Vulkan clear color setting
    }

    void VulkanRendererAPI::Clear()
    {
        // TODO(Hachem): Implement Vulkan clear
    }

    void VulkanRendererAPI::EnableDepthTest()
    {
        // TODO(Hachem): Implement Vulkan depth test enabling
    }

    void VulkanRendererAPI::DisableDepthTest()
    {
        // TODO(Hachem): Implement Vulkan depth test disabling
    }

    void VulkanRendererAPI::SetFaceCulling(bool enabled)
    {
        // TODO(Hachem): Implement Vulkan face culling setting
    }

    void VulkanRendererAPI::EnableBlending()
    {
        // TODO(Hachem): Implement Vulkan blending enabling
    }

    void VulkanRendererAPI::DisableBlending()
    {
        // TODO(Hachem): Implement Vulkan blending disabling
    }

    void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        // TODO(Hachem): Implement Vulkan indexed drawing
    }

    void VulkanRendererAPI::DrawArrays(uint32_t vertexCount, uint32_t first)
    {
        // TODO(Hachem): Implement Vulkan array drawing
    }

    void VulkanRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        // TODO(Hachem): Implement Vulkan line drawing
    }

    void VulkanRendererAPI::BindTexture(uint32_t textureID, uint32_t slot)
    {
        // TODO(Hachem): Implement Vulkan texture binding
    }

    void VulkanRendererAPI::BindImageTexture(uint32_t textureID, uint32_t slot, bool readOnly)
    {
        // TODO(Hachem): Implement Vulkan image texture binding
    }
};

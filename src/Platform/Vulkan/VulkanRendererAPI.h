#pragma once

#include "Core/Memory.h"
#include "Rendering/Renderer.h"

namespace Donut
{
    class VulkanRendererAPI 
        : public RendererAPI
    {
    public:
        virtual void Init()                                       override;
        virtual void SetViewport(uint32_t x,     uint32_t y, 
                                 uint32_t width, uint32_t height) override;
        virtual void SetClearColor(const glm::vec4& color)        override;
        virtual void Clear()                                      override;
        virtual void EnableDepthTest()                            override;
        virtual void DisableDepthTest()                           override;
        virtual void SetFaceCulling(bool enabled)                 override;
        virtual void EnableBlending()                             override;
        virtual void DisableBlending()                            override;

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, 
                                 uint32_t indexCount = 0)         override;
        
        virtual void DrawArrays(uint32_t vertexCount, 
                                uint32_t first = 0)               override;
        virtual void DrawLines(const Ref<VertexArray>& vertexArray, 
                               uint32_t indexCount = 0)           override;
        virtual void BindTexture(uint32_t textureID, 
                                 uint32_t slot = 0)               override;
        virtual void BindImageTexture(uint32_t textureID, 
                                      uint32_t slot = 0, 
                                      bool readOnly = false)      override;
    };
};

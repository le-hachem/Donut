#pragma once

#include "Core/Memory.h"
#include "Rendering/Texture.h"
#include <cstdint>

namespace Donut
{
    class TextureManager
    {
    public:
        static Ref<Texture2D> CreateTexture(uint32_t width, uint32_t height);
        
        static void BindTexture(uint32_t textureID, uint32_t slot = 0);
        static void BindImageTexture(uint32_t textureID, uint32_t slot = 0, bool readOnly = false);
        static void SetTextureData(uint32_t textureID, void* data, uint32_t width, uint32_t height);
        static void ResizeTexture(uint32_t textureID, uint32_t width, uint32_t height);
    };
};

#include "TextureManager.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace Donut
{
    Ref<Texture2D> TextureManager::CreateTexture(uint32_t width, uint32_t height)
    {
        return CreateRef<OpenGLTexture2D>(width, height);
    }

    void TextureManager::BindTexture(uint32_t textureID, uint32_t slot)
    {
        RenderCommand::BindTexture(textureID, slot);
    }

    void TextureManager::BindImageTexture(uint32_t textureID, uint32_t slot, bool readOnly)
    {
        RenderCommand::BindImageTexture(textureID, slot, readOnly);
    }

    void TextureManager::SetTextureData(uint32_t textureID, void* data, uint32_t width, uint32_t height)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    void TextureManager::ResizeTexture(uint32_t textureID, uint32_t width, uint32_t height)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
};

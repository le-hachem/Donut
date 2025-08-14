#include "Shader.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Donut
{
    Ref<Shader> Shader::Create(const std::string& filepath) 
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::OpenGL:
                return CreateRef<OpenGLShader>(filepath);
            case RendererAPI::API::Vulkan:
                return CreateRef<VulkanShader>(filepath);
            default:
                return nullptr;
        }
    }

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
            case RendererAPI::API::Vulkan:
                return CreateRef<VulkanShader>(name, vertexSrc, fragmentSrc);
            default:
                return nullptr;
        }
    }

    Ref<Shader> Shader::CreateCompute(const std::string& name, const std::string& computeSrc)
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return CreateRef<OpenGLShader>(name, computeSrc);
            case RendererAPI::API::Vulkan:
                return CreateRef<VulkanShader>(name, computeSrc);
            default:
                return nullptr;
        }
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        auto& name = shader->GetName();
        Add(name, shader);
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        m_Shaders[name] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        if (Exists(name))
            return m_Shaders[name];
        return nullptr;
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }
};

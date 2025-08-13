#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <memory>

namespace Donut
{
    Shader* Shader::Create(const std::string& filepath) 
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::OpenGL:
                return new OpenGLShader(filepath);
            default:
                return nullptr;
        }
    }

    Shader* Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return new OpenGLShader(name, vertexSrc, fragmentSrc);
            default:
                return nullptr;
        }
    }

    Shader* Shader::CreateCompute(const std::string& name, const std::string& computeSrc)
    {
        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::OpenGL:
                return new OpenGLShader(name, computeSrc);
            default:
                return nullptr;
        }
    }

    void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
    {
        auto& name = shader->GetName();
        Add(name, shader);
    }

    void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
    {
        m_Shaders[name] = shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath)
    {
        auto shader = std::shared_ptr<Shader>(Shader::Create(filepath));
        Add(shader);
        return shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    {
        auto shader = std::shared_ptr<Shader>(Shader::Create(filepath));
        Add(name, shader);
        return shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
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

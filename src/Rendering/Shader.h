#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

#define SHADER_STORAGE_BARRIER_BIT    0x00002000
#define UNIFORM_BARRIER_BIT           0x00000004
#define TEXTURE_FETCH_BARRIER_BIT     0x00000008
#define IMAGE_ACCESS_BARRIER_BIT      0x00000020

namespace Donut 
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(     const std::string& name, int value)                   = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
        virtual void SetFloat(   const std::string& name, float value)                 = 0;
        virtual void SetFloat2(  const std::string& name, const glm::vec2& value)      = 0;
        virtual void SetFloat3(  const std::string& name, const glm::vec3& value)      = 0;
        virtual void SetFloat4(  const std::string& name, const glm::vec4& value)      = 0;
        virtual void SetMat4(    const std::string& name, const glm::mat4& value)      = 0;

        virtual void Dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) = 0;
        virtual void DispatchIndirect(uint32_t offset = 0)                = 0;
        virtual void MemoryBarrier(uint32_t barriers)                     = 0;

        virtual const std::string& GetName() const = 0;

        static Shader* Create(const std::string& filepath);
        static Shader* Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        static Shader* CreateCompute(const std::string& name, const std::string& computeSrc);
    };

    class ShaderLibrary
    {
    public:
        void Add(const std::shared_ptr<Shader>& shader);
        void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
        std::shared_ptr<Shader> Load(const std::string& filepath);
        std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepath);

        std::shared_ptr<Shader> Get(const std::string& name);

        bool Exists(const std::string& name) const;
    private:
        std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
    };
};

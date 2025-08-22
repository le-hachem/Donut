#pragma once

#include "Core/Log.h"
#include "Rendering/Texture.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace Donut
{
    class HDRIManager
    {
    public:
        static HDRIManager& Get()
        {
            static HDRIManager instance;
            return instance;
        }

        Ref<CubemapTexture> LoadHDRI(const std::string& path);
        Ref<CubemapTexture> GetCurrentHDRI() const { return m_CurrentHDRI; }
        
        void SetCurrentHDRI(const std::string& path);
        const std::vector<std::string>& GetAvailableHDRI() const { return m_AvailableHDRI; }
        std::string GetHDRIName(const std::string& path)   const;
        void ClearCache();
    private:
         HDRIManager() = default;
        ~HDRIManager() = default;

        HDRIManager(const HDRIManager&)            = delete;
        HDRIManager& operator=(const HDRIManager&) = delete;

        std::unordered_map<std::string, Ref<CubemapTexture>> m_HDRICache;
        
        Ref<CubemapTexture> m_CurrentHDRI;
        
        std::vector<std::string> m_AvailableHDRI = 
        {
            "Assets/HDRI/HDR_blue_nebulae-1.hdr",
            "Assets/HDRI/HDR_subdued_blue_nebulae.hdr",
            "Assets/HDRI/HDR_subdued_multi_nebulae.hdr"
        };
    };
};

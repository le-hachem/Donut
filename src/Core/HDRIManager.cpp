#include "HDRIManager.h"
#include <filesystem>

namespace Donut
{
    Ref<CubemapTexture> HDRIManager::LoadHDRI(const std::string& path)
    {
        auto it = m_HDRICache.find(path);
        if (it != m_HDRICache.end())
        {
            DONUT_INFO("HDRI already cached: {}", path);
            return it->second;
        }

        DONUT_INFO("Loading HDRI: {}", path);
        auto hdri = CubemapTexture::CreateFromHDRI(path);
        
        if (hdri)
        {
            m_HDRICache[path] = hdri;
            DONUT_INFO("Successfully loaded and cached HDRI: {}", path);
        }
        else
            DONUT_ERROR("Failed to load HDRI: {}", path);

        return hdri;
    }

    void HDRIManager::SetCurrentHDRI(const std::string& path)
    {
        auto hdri = LoadHDRI(path);
        if (hdri)
        {
            m_CurrentHDRI = hdri;
            DONUT_INFO("Set current HDRI to: {}", path);
        }
        else
            DONUT_ERROR("Failed to set current HDRI: {}", path);
    }

    std::string HDRIManager::GetHDRIName(const std::string& path) const
    {
        std::filesystem::path filePath(path);
        std::string filename = filePath.stem().string();
        std::string name = filename;
        
        for (size_t i = 0; i < name.length(); ++i)
        {
            if (name[i] == '_')
                name[i] = ' ';
            else if (i == 0 || name[i-1] == ' ')
                name[i] = std::toupper(name[i]);
        }
        
        return name;
    }

    void HDRIManager::ClearCache()
    {
        m_HDRICache.clear();
        m_CurrentHDRI.reset();
        DONUT_INFO("HDRI cache cleared");
    }
};

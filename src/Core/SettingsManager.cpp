#include "SettingsManager.h"
#include "Log.h"
#include "ThemeManager.h"
#include "Rendering/Renderer.h"

#include <fstream>
#include <filesystem>
#include <algorithm>

namespace Donut
{
    Settings SettingsManager::s_Settings;
    bool     SettingsManager::s_Initialized = false;

    void SettingsManager::Initialize()
    {
        if (s_Initialized)
            return;
            
        LoadSettings();
        s_Initialized = true;
        DONUT_INFO("Settings Manager initialized");
    }

    void SettingsManager::Shutdown()
    {
        if (!s_Initialized)
            return;
            
        SaveSettings();
        s_Initialized = false;
        DONUT_INFO("Settings Manager shutdown");
    }

    void SettingsManager::LoadSettings()
    {
        std::string filePath = GetSettingsFilePath();
        
        try
        {
            if (std::filesystem::exists(filePath))
            {
                auto config = toml::parse(filePath);
                
                if (config.contains("simulation"))
                {
                    auto sim = config["simulation"];
                    s_Settings.simulation.targetFPS         = toml::find_or(sim, "target_fps",          60);
                    s_Settings.simulation.computeHeight     = toml::find_or(sim, "compute_height",      512);
                    s_Settings.simulation.maxStepsMoving    = toml::find_or(sim, "max_steps_moving",    30000);
                    s_Settings.simulation.maxStepsStatic    = toml::find_or(sim, "max_steps_static",    15000);
                    s_Settings.simulation.earlyExitDistance = toml::find_or(sim, "early_exit_distance", 5e12f);
                    s_Settings.simulation.gravityEnabled    = toml::find_or(sim, "gravity_enabled",     true);
                    
                    s_Settings.simulation.targetFPS         = std::max(30,    std::min(120,   s_Settings.simulation.targetFPS));
                    s_Settings.simulation.computeHeight     = std::max(64,    std::min(2048,  s_Settings.simulation.computeHeight));
                    s_Settings.simulation.maxStepsMoving    = std::max(1000,  std::min(60000, s_Settings.simulation.maxStepsMoving));
                    s_Settings.simulation.maxStepsStatic    = std::max(1000,  std::min(30000, s_Settings.simulation.maxStepsStatic));
                    s_Settings.simulation.earlyExitDistance = std::max(1e11f, std::min(1e13f, s_Settings.simulation.earlyExitDistance));
                }
                
                if (config.contains("graphics"))
                {
                    auto gfx = config["graphics"];
                    s_Settings.graphics.renderAPI              = toml::find_or(gfx, "render_api",               std::string("OpenGL"));
                    s_Settings.graphics.vSyncEnabled           = toml::find_or(gfx, "vsync_enabled",            true);
                    s_Settings.graphics.showFPS                = toml::find_or(gfx, "show_fps",                 true);
                    s_Settings.graphics.showPerformanceMetrics = toml::find_or(gfx, "show_performance_metrics", true);
                    s_Settings.graphics.showDebugInfo          = toml::find_or(gfx, "show_debug_info",          false);
                    s_Settings.graphics.enableAntiAliasing     = toml::find_or(gfx, "enable_anti_aliasing",     true);
                    s_Settings.graphics.selectedTheme          = toml::find_or(gfx, "selected_theme",           std::string("Dark"));
                    
                    if (s_Settings.graphics.renderAPI != "OpenGL" && 
                        s_Settings.graphics.renderAPI != "Vulkan")
                        s_Settings.graphics.renderAPI = "OpenGL";
                    if (s_Settings.graphics.selectedTheme != "Dark" && 
                        s_Settings.graphics.selectedTheme != "Light" && 
                        s_Settings.graphics.selectedTheme != "Blue")
                        s_Settings.graphics.selectedTheme = "Dark";
                }
                
                DONUT_INFO("Settings loaded from {}", filePath);
            }
            else
            {
                LoadDefaultSettings();
                SaveSettings();
                DONUT_INFO("No settings file found, created default settings");
            }
        }
        catch (const std::exception& e)
        {
            DONUT_ERROR("Failed to load settings: {}", e.what());
            LoadDefaultSettings();
        }
    }

    void SettingsManager::SaveSettings()
    {
        std::string filePath = GetSettingsFilePath();
        
        try
        {
            std::filesystem::path path(filePath);
            std::filesystem::create_directories(path.parent_path());
            
            toml::value simulation = toml::table
            {
                {"target_fps",          s_Settings.simulation.targetFPS        },
                {"compute_height",      s_Settings.simulation.computeHeight    },
                {"max_steps_moving",    s_Settings.simulation.maxStepsMoving   },
                {"max_steps_static",    s_Settings.simulation.maxStepsStatic   },
                {"early_exit_distance", s_Settings.simulation.earlyExitDistance},
                {"gravity_enabled",     s_Settings.simulation.gravityEnabled   }
            };
            
            toml::value graphics = toml::table
            {
                {"render_api",               s_Settings.graphics.renderAPI             },
                {"vsync_enabled",            s_Settings.graphics.vSyncEnabled          },
                {"show_fps",                 s_Settings.graphics.showFPS               },
                {"show_performance_metrics", s_Settings.graphics.showPerformanceMetrics},
                {"show_debug_info",          s_Settings.graphics.showDebugInfo         },
                {"enable_anti_aliasing",     s_Settings.graphics.enableAntiAliasing    },
                {"selected_theme",           s_Settings.graphics.selectedTheme         }
            };
            
            toml::value config = toml::table
            {
                {"simulation", simulation},
                {"graphics",   graphics}
            };
            
            std::ofstream file(filePath);
            file << config;
            file.close();
            
            DONUT_INFO("Settings saved to {}", filePath);
        }
        catch (const std::exception& e)
            DONUT_ERROR("Failed to save settings: {}", e.what());
    }

    void SettingsManager::SetSimulationSettings(const SimulationSettings& settings)
    {
        s_Settings.simulation = settings;
        SaveSettings();
    }

    void SettingsManager::SetGraphicsSettings(const GraphicsSettings& settings)
    {
        s_Settings.graphics = settings;
        SaveSettings();
    }

    std::string SettingsManager::GetSettingsFilePath()
    {
        return "Config/settings.toml";
    }

    void SettingsManager::LoadDefaultSettings()
    {
        s_Settings.simulation.targetFPS         = 60;
        s_Settings.simulation.computeHeight     = 512;
        s_Settings.simulation.maxStepsMoving    = 30000;
        s_Settings.simulation.maxStepsStatic    = 15000;
        s_Settings.simulation.earlyExitDistance = 5e12f;
        s_Settings.simulation.gravityEnabled    = true;
        
        s_Settings.graphics.renderAPI              = "OpenGL";
        s_Settings.graphics.vSyncEnabled           = true;
        s_Settings.graphics.showFPS                = true;
        s_Settings.graphics.showPerformanceMetrics = true;
        s_Settings.graphics.showDebugInfo          = false;
        s_Settings.graphics.enableAntiAliasing     = true;
        s_Settings.graphics.selectedTheme          = "Dark";
    }
}

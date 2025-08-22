#pragma once

#include <string>
#include <toml.hpp>

namespace Donut
{
    struct SimulationSettings
    {
        int   targetFPS         = 60;
        int   computeHeight     = 512;
        int   maxStepsMoving    = 30000;
        int   maxStepsStatic    = 15000;
        float earlyExitDistance = 5e12f;
        bool  gravityEnabled    = true;
        float diskThickness     = 0.1f;
        float diskDensity       = 0.1f;
        float rotationSpeed     = 1.0f;
        float blurStrength      = 2.0f;
        float glowIntensity     = 0.1f;
    };

    struct GraphicsSettings
    {
        std::string renderAPI              = "OpenGL";
        bool        vSyncEnabled           = true;
        bool        showFPS                = true;
        bool        showPerformanceMetrics = true;
        bool        showDebugInfo          = false;
        bool        enableAntiAliasing     = true;
        std::string selectedTheme          = "Dark";
    };

    struct Settings
    {
        SimulationSettings simulation;
        GraphicsSettings   graphics;
    };

    class SettingsManager
    {
    public:
        static void Initialize();
        static void Shutdown();
        
        static void LoadSettings();
        static void SaveSettings();
        
        static       Settings& GetSettings()      { return s_Settings; }
        static const Settings& GetSettingsConst() { return s_Settings; }
        
        static void SetSimulationSettings(const SimulationSettings& settings);
        static void SetGraphicsSettings(const GraphicsSettings& settings);

        static int   GetTargetFPS()              { return s_Settings.simulation.targetFPS;            }
        static int   GetComputeHeight()          { return s_Settings.simulation.computeHeight;        }
        static int   GetMaxStepsMoving()         { return s_Settings.simulation.maxStepsMoving;       }
        static int   GetMaxStepsStatic()         { return s_Settings.simulation.maxStepsStatic;       }
        static float GetEarlyExitDistance()      { return s_Settings.simulation.earlyExitDistance;    }
        static bool  GetGravityEnabled()         { return s_Settings.simulation.gravityEnabled;       }
        static float GetDiskThickness()          { return s_Settings.simulation.diskThickness;        }
        static float GetDiskDensity()            { return s_Settings.simulation.diskDensity;          }
        static float GetRotationSpeed()          { return s_Settings.simulation.rotationSpeed;        }
        static float GetBlurStrength()           { return s_Settings.simulation.blurStrength;         }
        static float GetGlowIntensity()          { return s_Settings.simulation.glowIntensity;        }
        static std::string GetRenderAPI()        { return s_Settings.graphics.renderAPI;              }
        static bool  GetVSyncEnabled()           { return s_Settings.graphics.vSyncEnabled;           }
        static bool  GetShowFPS()                { return s_Settings.graphics.showFPS;                }
        static bool  GetShowPerformanceMetrics() { return s_Settings.graphics.showPerformanceMetrics; }
        static bool  GetShowDebugInfo()          { return s_Settings.graphics.showDebugInfo;          }
        static bool  GetEnableAntiAliasing()     { return s_Settings.graphics.enableAntiAliasing;     }
        static std::string GetSelectedTheme()    { return s_Settings.graphics.selectedTheme;          }
    private:
        static std::string GetSettingsFilePath();
        static void        LoadDefaultSettings();
    private:
        static Settings s_Settings;
        static bool     s_Initialized;
    };
}

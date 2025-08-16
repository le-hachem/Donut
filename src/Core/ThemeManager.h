#pragma once

#include <imgui.h>

namespace Donut
{
    enum class Theme
    {
        Dark = 0,
        Light = 1,
        Blue = 2
    };

    class ThemeManager
    {
    public:
        static void SetTheme(Theme theme);
        static Theme GetCurrentTheme() { return s_CurrentTheme; }
        
        static void ApplyDarkTheme();
        static void ApplyLightTheme();
        static void ApplyBlueTheme();
        
    private:
        static Theme s_CurrentTheme;
    };
}

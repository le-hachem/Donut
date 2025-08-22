#pragma once

#include "Core/State.h"
#include "Core/Event.h"
#include "Core/Log.h"
#include "Rendering/Renderer.h"

namespace Donut
{
    class ConfigState
        : public State
    {
    public:
        ~ConfigState() = default;
        
        void OnEnter()                 override;
        void OnExit()                  override;
        void OnUpdate(float deltaTime) override;
        void OnRender()                override;
        void OnImUIRender()            override;
        void OnEvent(Event& event)     override;
        
    private:
        void ApplySettings();
        void ResetToDefaults();
        
    private:
        RendererAPI::API m_SelectedAPI = RendererAPI::API::OpenGL;
        bool m_ShowRestartMessage = false;
        float m_RestartMessageTimer = 0.0f;
        
        int   m_TargetFPS         = 60;
        int   m_ComputeHeight     = 512;
        int   m_MaxStepsMoving    = 30000;
        int   m_MaxStepsStatic    = 15000;
        float m_EarlyExitDistance = 5e12f;
        bool  m_GravityEnabled    = true;
        
        bool m_VSyncEnabled = true;
        bool m_ShowFPS = true;
        bool m_ShowPerformanceMetrics = true;
        bool m_ShowDebugInfo = false;
        bool m_EnableAntiAliasing = true;
        
        int m_SelectedTheme = 0; // 0=Dark, 1=Light, 2=Blue
    };
};
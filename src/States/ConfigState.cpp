#include "ConfigState.h"
#include "Rendering/Renderer.h"
#include "Core/Application.h"
#include "Core/ThemeManager.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glad/glad.h>

namespace Donut
{
    void ConfigState::OnEnter()
    {
        DONUT_INFO("Entering Config State");
        
        m_SelectedAPI   = Renderer::GetAPI();
        m_SelectedTheme = static_cast<int>(ThemeManager::GetCurrentTheme());
    }
    
    void ConfigState::OnExit()
    {
        DONUT_INFO("Exiting Config State");
    }
    
    void ConfigState::OnUpdate(float deltaTime)
    {
        if (m_ShowRestartMessage)
        {
            m_RestartMessageTimer += deltaTime;
            if (m_RestartMessageTimer > 3.0f)
            {
                m_ShowRestartMessage = false;
                m_RestartMessageTimer = 0.0f;
            }
        }
    }
    
    void ConfigState::OnRender()
    {
        Renderer::SetClearColor({ 0.05f, 0.05f, 0.1f, 1.0f });
        Renderer::Clear();
    }
    
    void ConfigState::OnEvent(Event& event)
    {
    }
    
    void ConfigState::OnImUIRender()
    {
        ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
                               ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        
        ImGui::Begin("Donut Configuration", nullptr, ImGuiWindowFlags_NoCollapse);
        
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Donut Configuration");
        ImGui::PopFont();
        ImGui::Separator();
        
        ImGui::Columns(2, "ConfigColumns", true);
        
        float availableHeight = ImGui::GetWindowHeight() - 140; // Reserve space for header, separator, buttons, and footer
        
        ImGui::BeginChild("GraphicsSettings", ImVec2(0, availableHeight), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Graphics Settings");
        ImGui::Separator();
        
        ImGui::Text("Render API:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(requires restart)");
        
        const char* apiNames[] = { "OpenGL", "Vulkan" };
        static int currentAPI = (int)m_SelectedAPI - 1; 
        
        if (ImGui::BeginCombo("##RenderAPI", apiNames[currentAPI]))
        {
            for (int i = 0; i < IM_ARRAYSIZE(apiNames); i++)
            {
                const bool isSelected = (currentAPI == i);
                if (ImGui::Selectable(apiNames[i], isSelected))
                {
                    currentAPI = i;
                    m_SelectedAPI = (RendererAPI::API)(i + 1);
                    m_ShowRestartMessage = true;
                    m_RestartMessageTimer = 0.0f;
                }
                
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Current API: ");
        ImGui::SameLine();
        const char* currentAPIName = (Renderer::GetAPI() == RendererAPI::API::OpenGL) ? "OpenGL" : "Vulkan";
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), currentAPIName);
        
        ImGui::Spacing();
        
        ImGui::Checkbox("Enable VSync", &m_VSyncEnabled);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(recommended)");
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Display");
        ImGui::Separator();
        
        ImGui::Text("Window Size: 1280x720");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Fullscreen: Not implemented yet");
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "System Info");
        ImGui::Separator();
        
        ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
        ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Performance");
        ImGui::Separator();
        
        ImGui::SliderInt("Target FPS", &m_TargetFPS, 30, 120, "%d FPS");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Target frame rate for the simulation");
            
        ImGui::Checkbox("Show FPS Counter", &m_ShowFPS);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Display current FPS in the simulation");
            
        ImGui::Checkbox("Show Performance Metrics", &m_ShowPerformanceMetrics);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Show detailed performance information");
            
        ImGui::Checkbox("Show Debug Info", &m_ShowDebugInfo);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Display debug information and statistics");
            
        ImGui::Checkbox("Enable Anti-Aliasing", &m_EnableAntiAliasing);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Enable anti-aliasing for smoother rendering");
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Theme");
        ImGui::Separator();
        
        const char* themeNames[] = { "Dark", "Light", "Blue" };
        if (ImGui::Combo("UI Theme", &m_SelectedTheme, themeNames, IM_ARRAYSIZE(themeNames)))
            ThemeManager::SetTheme(static_cast<Theme>(m_SelectedTheme));
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Choose the application theme");
        
        ImGui::EndChild();
        
        ImGui::NextColumn();
        
        ImGui::BeginChild("SimulationSettings", ImVec2(0, availableHeight), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Simulation Settings");
        ImGui::Separator();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Quality");
        ImGui::Separator();
        
        ImGui::SliderInt("Compute Height", &m_ComputeHeight, 64, 2048, "%d px");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Resolution of the compute shader. Higher values give better quality but lower performance.");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Higher = better quality, lower performance");
        
        ImGui::SliderInt("Max Steps (Moving)", &m_MaxStepsMoving, 1000, 60000, "%d");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Maximum ray marching steps when camera is moving");
            
        ImGui::SliderInt("Max Steps (Static)", &m_MaxStepsStatic, 1000, 30000, "%d");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Maximum ray marching steps when camera is stationary");
        
        ImGui::SliderFloat("Early Exit Distance", &m_EarlyExitDistance, 1e11f, 1e13f, "%.2e");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Distance at which ray marching stops to improve performance");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Distance at which ray marching stops");
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Physics");
        ImGui::Separator();
        
        ImGui::Checkbox("Enable Gravity", &m_GravityEnabled);
        
        ImGui::EndChild();
        
        ImGui::Columns(1);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float buttonWidth = (ImGui::GetWindowWidth() - 80) / 4.0f;
        
        if (ImGui::Button("Start Simulation", ImVec2(buttonWidth, 35)))
        {
            ApplySettings();
            Application::Get().GetStateManager().SwitchToState("Simulation");
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Reset to Defaults", ImVec2(buttonWidth, 35)))
            ResetToDefaults();
        
        ImGui::SameLine();
        if (ImGui::Button("Apply Settings", ImVec2(buttonWidth, 35)))
            ApplySettings();
        
        ImGui::SameLine();
        if (ImGui::Button("Exit", ImVec2(buttonWidth, 35)))
            Application::Get().Close();
        
        if (m_ShowRestartMessage)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::TextWrapped("Warning: Render API changed! Please restart the application for changes to take effect.");
            ImGui::PopStyleColor();
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Donut Engine v1.0.0 | Black Hole Simulation");
        
        ImGui::End();
    }
    
    void ConfigState::ApplySettings()
    {
        RendererAPI::SetAPI(m_SelectedAPI);
        DONUT_INFO("Settings applied");
    }
    
    void ConfigState::ResetToDefaults()
    {
        m_SelectedAPI            = RendererAPI::API::OpenGL;
        m_TargetFPS              = 60;
        m_ComputeHeight          = 512;
        m_MaxStepsMoving         = 30000;
        m_MaxStepsStatic         = 15000;
        m_EarlyExitDistance      = 5e12f;
        m_GravityEnabled         = true;
        m_VSyncEnabled           = true;
        m_ShowFPS                = true;
        m_ShowPerformanceMetrics = true;
        m_ShowDebugInfo          = false;
        m_EnableAntiAliasing     = true;
        m_SelectedTheme          = 0;
        
        DONUT_INFO("Settings reset to defaults");
    }
};

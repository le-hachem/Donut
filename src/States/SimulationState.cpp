#include "SimulationState.h"
#include "Rendering/Renderer.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Event.h"
#include "Core/SettingsManager.h"

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Donut
{
    void SimulationState::OnEnter()
    {
        DONUT_INFO("Entering Simulation State");
        
        const auto& settings = SettingsManager::GetSettingsConst();
        auto& engine = Application::Get().GetEngine();
        
        engine.SetTargetFPS(settings.simulation.targetFPS);
        engine.SetComputeHeight(settings.simulation.computeHeight);
        engine.SetMaxStepsMoving(settings.simulation.maxStepsMoving);
        engine.SetMaxStepsStatic(settings.simulation.maxStepsStatic);
        engine.SetEarlyExitDistance(settings.simulation.earlyExitDistance);
        engine.GetGravity() = settings.simulation.gravityEnabled;
        
        engine.UpdateComputeDimensions();
        m_Initialized = true;
    }
    
    void SimulationState::OnExit()
    {
        DONUT_INFO("Exiting Simulation State");
    }
    
    void SimulationState::OnUpdate(float deltaTime)
    {
        auto& engine = Application::Get().GetEngine();
        engine.UpdatePerformance(deltaTime);
        engine.UpdateWindowDimensions();
        engine.UpdatePhysics(deltaTime);

        if (engine.GetCamera().IsDragging())
        {
            GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            engine.GetCamera().ProcessOrbitalMouseMove(xpos, ypos);
        }
    }
    
    void SimulationState::OnRender()
    {
        auto& engine = Application::Get().GetEngine();
        RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        RenderCommand::Clear();
        RenderCommand::SetViewport(0, 0, static_cast<uint32_t>(engine.GetWidth()), static_cast<uint32_t>(engine.GetHeight()));

        engine.DispatchCompute(engine.GetCamera());
        engine.DrawBlurPass();
    }
    
    void SimulationState::OnEvent(Event& event)
    {
        auto& engine = Application::Get().GetEngine();
        
        if (event.GetEventType() == EventType::MouseButtonPressed)
        {
            MouseButtonPressedEvent& e = (MouseButtonPressedEvent&)event;
            int button = e.GetMouseButton();
            
            GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            Camera& camera = engine.GetCamera();
            double lastX = camera.GetLastX();
            double lastY = camera.GetLastY();
            glfwGetCursorPos(window, &lastX, &lastY);
            
            camera.ProcessOrbitalMouseButton(button, GLFW_PRESS, 0);
        }
        else if (event.GetEventType() == EventType::MouseButtonReleased)
        {
            MouseButtonReleasedEvent& e = (MouseButtonReleasedEvent&)event;
            int button = e.GetMouseButton();
            engine.GetCamera().ProcessOrbitalMouseButton(button, GLFW_RELEASE, 0);
        }
        
        if (event.GetEventType() == EventType::MouseScrolled)
        {
            MouseScrolledEvent& e = (MouseScrolledEvent&)event;
            engine.GetCamera().ProcessOrbitalScroll(e.GetXOffset(), e.GetYOffset());
        }
        
        if (event.GetEventType() == EventType::KeyPressed)
        {
            KeyPressedEvent& e = (KeyPressedEvent&)event;
            if (e.GetKeyCode() == GLFW_KEY_G)
            {
                engine.GetGravity() = !engine.GetGravity();
                DONUT_INFO("Gravity turned {}", engine.GetGravity() ? "ON" : "OFF");
            }
        }
    }
    
    void SimulationState::OnImUIRender()
    {
        auto& engine = Application::Get().GetEngine();
        
        ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 420, 20), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Simulation Controls", nullptr, ImGuiWindowFlags_NoCollapse);
        
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Black Hole Simulation");
        ImGui::SameLine();
        if (ImGui::Button("Back to Config"))
            Application::Get().GetStateManager().SwitchToState("Config");
        ImGui::SameLine();
        if (ImGui::Button("Save Settings"))
        {
            SimulationSettings settings = SettingsManager::GetSettingsConst().simulation;
            settings.targetFPS = engine.GetTargetFPS();
            settings.computeHeight = engine.GetComputeHeight();
            settings.maxStepsMoving = engine.GetMaxStepsMoving();
            settings.maxStepsStatic = engine.GetMaxStepsStatic();
            settings.earlyExitDistance = engine.GetEarlyExitDistance();
            settings.gravityEnabled = engine.GetGravity();
            SettingsManager::SetSimulationSettings(settings);
            DONUT_INFO("Simulation settings saved manually");
        }

        ImGui::Separator();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Performance");
        ImGui::Separator();
        
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("Engine FPS: %.1f", engine.GetCurrentFPS());
        
        int targetFPS = engine.GetTargetFPS();
        if (ImGui::SliderInt("Target FPS", &targetFPS, 30, 120))
        {
            engine.SetTargetFPS(targetFPS);
            SimulationSettings settings = SettingsManager::GetSettingsConst().simulation;
            settings.targetFPS = targetFPS;
            SettingsManager::SetSimulationSettings(settings);
        }
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Simulation Info");
        ImGui::Separator();
        
        ImGui::Text("Resolution: %dx%d", engine.GetWidth(), engine.GetHeight());
        ImGui::Text("Compute Resolution: %dx%d", engine.GetComputeWidth(), engine.GetComputeHeight());
        ImGui::Text("Objects: %zu", engine.GetObjects().size());
        
        if (ImGui::Button("Print Object Info"))
        {
            engine.PrintObjectInfo();
        }
        
        int computeHeight = engine.GetComputeHeight();
        if (ImGui::SliderInt("Compute Height", &computeHeight, 64, 2048))
        {
            engine.SetComputeHeight(computeHeight);
            engine.UpdateComputeDimensions();
            SimulationSettings settings = SettingsManager::GetSettingsConst().simulation;
            settings.computeHeight = computeHeight;
            SettingsManager::SetSimulationSettings(settings);
        }
        
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Compute Width: %d (auto-calculated)", engine.GetComputeWidth());
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Quality Settings");
        ImGui::Separator();
        
        int maxStepsMoving = engine.GetMaxStepsMoving();
        if (ImGui::SliderInt("Max Steps (Moving)", &maxStepsMoving, 1000, 60000))
        {
            engine.SetMaxStepsMoving(maxStepsMoving);
            SimulationSettings settings = SettingsManager::GetSettingsConst().simulation;
            settings.maxStepsMoving = maxStepsMoving;
            SettingsManager::SetSimulationSettings(settings);
        }
        int maxStepsStatic = engine.GetMaxStepsStatic();
        if (ImGui::SliderInt("Max Steps (Static)", &maxStepsStatic, 1000, 30000))
        {
            engine.SetMaxStepsStatic(maxStepsStatic);
            SimulationSettings settings = SettingsManager::GetSettingsConst().simulation;
            settings.maxStepsStatic = maxStepsStatic;
            SettingsManager::SetSimulationSettings(settings);
        }
        float earlyExitDistance = engine.GetEarlyExitDistance();
        if (ImGui::SliderFloat("Early Exit Distance", &earlyExitDistance, 1e11f, 1e13f, "%.2e"))
        {
            engine.SetEarlyExitDistance(earlyExitDistance);
            SimulationSettings settings = SettingsManager::GetSettingsConst().simulation;
            settings.earlyExitDistance = earlyExitDistance;
            SettingsManager::SetSimulationSettings(settings);
        }
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Physics");
        ImGui::Separator();
        
        bool& gravity = engine.GetGravity();
        if (ImGui::Checkbox("Gravity Enabled", &gravity))
        {
            SimulationSettings settings = SettingsManager::GetSettingsConst().simulation;
            settings.gravityEnabled = gravity;
            SettingsManager::SetSimulationSettings(settings);
        }
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Accretion Disk");
        ImGui::Separator();
        
        float diskThickness = engine.GetDiskThickness();
        if (ImGui::SliderFloat("Cloud Thickness", &diskThickness, 0.1f, 2.0f, "%.2f"))
        {
            engine.SetDiskThickness(diskThickness);
        }
        ImGui::TextDisabled("Thickness relative to Schwarzschild radius");
        
        float diskDensity = engine.GetDiskDensity();
        if (ImGui::SliderFloat("Cloud Density", &diskDensity, 0.1f, 3.0f, "%.2f"))
        {
            engine.SetDiskDensity(diskDensity);
        }
        ImGui::TextDisabled("Overall density multiplier");
        
        float rotationSpeed = engine.GetRotationSpeed();
        if (ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 3.0f, "%.2f"))
        {
            engine.SetRotationSpeed(rotationSpeed);
        }
        ImGui::TextDisabled("Rotation speed multiplier (0 = no rotation)");
        
        float blurStrength = engine.GetBlurStrength();
        if (ImGui::SliderFloat("Blur Strength", &blurStrength, 0.5f, 5.0f, "%.2f"))
        {
            engine.SetBlurStrength(blurStrength);
        }
        ImGui::TextDisabled("Blur radius for glow effect");
        
        float glowIntensity = engine.GetGlowIntensity();
        if (ImGui::SliderFloat("Glow Intensity", &glowIntensity, 0.1f, 3.0f, "%.2f"))
        {
            engine.SetGlowIntensity(glowIntensity);
        }
        ImGui::TextDisabled("Intensity of the glow effect");
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Camera");
        ImGui::Separator();
        
        ImGui::Text("Position: (%.2e, %.2e, %.2e)", 
                   engine.GetCamera().GetOrbitalPosition().x, 
                   engine.GetCamera().GetOrbitalPosition().y, 
                   engine.GetCamera().GetOrbitalPosition().z);
        ImGui::Text("Radius: %.2e", engine.GetCamera().GetOrbitalRadius());
        ImGui::Text("Azimuth: %.2f", engine.GetCamera().GetAzimuth());
        ImGui::Text("Elevation: %.2f", engine.GetCamera().GetElevation());
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Export");
        ImGui::Separator();
        
        if (ImGui::Button("Export Frame (1080p)", ImVec2(-1, 30)))
        {
            auto& engine = Application::Get().GetEngine();
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "frame_1080p_" << std::put_time(std::localtime(&time_t), "%Y-%m-%d_%H-%M-%S") << ".png";
            engine.ExportHighResFrame(ss.str(), 1920, 1080);
        }
        
        if (ImGui::Button("Export High-Res Frame (4K)", ImVec2(-1, 30)))
        {
            auto& engine = Application::Get().GetEngine();
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "high_res_frame_4k_" << std::put_time(std::localtime(&time_t), "%Y-%m-%d_%H-%M-%S") << ".png";
            engine.ExportHighResFrame(ss.str(), 4096, 3072);
        }
        
        if (ImGui::Button("Export High-Res Frame (8K)", ImVec2(-1, 30)))
        {
            auto& engine = Application::Get().GetEngine();
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "high_res_frame_8k_" << std::put_time(std::localtime(&time_t), "%Y-%m-%d_%H-%M-%S") << ".png";
            engine.ExportHighResFrame(ss.str(), 8192, 6144);
        }
        
        if (ImGui::Button("Export Ultra High-Res Frame (16K)", ImVec2(-1, 30)))
        {
            auto& engine = Application::Get().GetEngine();
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "high_res_frame_16k_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".png";
            engine.ExportHighResFrame(ss.str(), 16384, 12288);
        }
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Controls");
        ImGui::Separator();
        
        ImGui::Text("Left Mouse: Orbit camera");
        ImGui::Text("Scroll: Zoom in/out");
        ImGui::Text("G: Toggle gravity");
        ImGui::Text("Right Mouse: Enable gravity (hold)");
        
        ImGui::End();
    }
};

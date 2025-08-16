#include "SimulationState.h"
#include "Rendering/Renderer.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Event.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

namespace Donut
{
    void SimulationState::OnEnter()
    {
        DONUT_INFO("Entering Simulation State");
        m_Initialized = true;
    }
    
    void SimulationState::OnExit()
    {
        DONUT_INFO("Exiting Simulation State");
    }
    
    void SimulationState::OnUpdate(float deltaTime)
    {
        m_Engine.UpdatePerformance(deltaTime);
        m_Engine.UpdateWindowDimensions();
        m_Engine.UpdatePhysics(deltaTime);

        if (m_Engine.GetCamera().IsDragging())
        {
            GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            m_Engine.GetCamera().ProcessOrbitalMouseMove(xpos, ypos);
        }
    }
    
    void SimulationState::OnRender()
    {
        RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        RenderCommand::Clear();
        RenderCommand::SetViewport(0, 0, m_Engine.GetWidth(), m_Engine.GetHeight());

        m_Engine.DispatchCompute(m_Engine.GetCamera());
        m_Engine.DrawFullScreenQuad();
    }
    
    void SimulationState::OnEvent(Event& event)
    {
        if (event.GetEventType() == EventType::MouseButtonPressed)
        {
            MouseButtonPressedEvent& e = (MouseButtonPressedEvent&)event;
            int button = e.GetMouseButton();
            
            GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            Camera& camera = m_Engine.GetCamera();
            double lastX = camera.GetLastX();
            double lastY = camera.GetLastY();
            glfwGetCursorPos(window, &lastX, &lastY);
            
            camera.ProcessOrbitalMouseButton(button, GLFW_PRESS, 0);
        }
        else if (event.GetEventType() == EventType::MouseButtonReleased)
        {
            MouseButtonReleasedEvent& e = (MouseButtonReleasedEvent&)event;
            int button = e.GetMouseButton();
            m_Engine.GetCamera().ProcessOrbitalMouseButton(button, GLFW_RELEASE, 0);
        }
        
        if (event.GetEventType() == EventType::MouseScrolled)
        {
            MouseScrolledEvent& e = (MouseScrolledEvent&)event;
            m_Engine.GetCamera().ProcessOrbitalScroll(e.GetXOffset(), e.GetYOffset());
        }
        
        if (event.GetEventType() == EventType::KeyPressed)
        {
            KeyPressedEvent& e = (KeyPressedEvent&)event;
            if (e.GetKeyCode() == GLFW_KEY_G)
            {
                m_Engine.GetGravity() = !m_Engine.GetGravity();
                DONUT_INFO("Gravity turned {}", m_Engine.GetGravity() ? "ON" : "OFF");
            }
        }
    }
    
    void SimulationState::OnImUIRender()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 420, 20), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Simulation Controls", nullptr, ImGuiWindowFlags_NoCollapse);
        
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Black Hole Simulation");
        if (ImGui::Button("Back to Config"))
            Application::Get().GetStateManager().SwitchToState("Config");

        ImGui::Separator();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Performance");
        ImGui::Separator();
        
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("Engine FPS: %.1f", m_Engine.GetCurrentFPS());
        
        int targetFPS = m_Engine.GetTargetFPS();
        if (ImGui::SliderInt("Target FPS", &targetFPS, 30, 120))
            m_Engine.SetTargetFPS(targetFPS);
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Simulation Info");
        ImGui::Separator();
        
        ImGui::Text("Resolution: %dx%d", m_Engine.GetWidth(), m_Engine.GetHeight());
        ImGui::Text("Compute Resolution: %dx%d", m_Engine.GetComputeWidth(), m_Engine.GetComputeHeight());
        ImGui::Text("Objects: %zu", m_Engine.GetObjects().size());
        
        int computeHeight = m_Engine.GetComputeHeight();
        if (ImGui::SliderInt("Compute Height", &computeHeight, 64, 2048))
        {
            m_Engine.SetComputeHeight(computeHeight);
            m_Engine.UpdateComputeDimensions();
        }
        
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Compute Width: %d (auto-calculated)", m_Engine.GetComputeWidth());
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Quality Settings");
        ImGui::Separator();
        
        int maxStepsMoving = m_Engine.GetMaxStepsMoving();
        if (ImGui::SliderInt("Max Steps (Moving)", &maxStepsMoving, 1000, 60000))
            m_Engine.SetMaxStepsMoving(maxStepsMoving);
        int maxStepsStatic = m_Engine.GetMaxStepsStatic();
        if (ImGui::SliderInt("Max Steps (Static)", &maxStepsStatic, 1000, 30000))
            m_Engine.SetMaxStepsStatic(maxStepsStatic);
        float earlyExitDistance = m_Engine.GetEarlyExitDistance();
        if (ImGui::SliderFloat("Early Exit Distance", &earlyExitDistance, 1e11f, 1e13f, "%.2e"))
            m_Engine.SetEarlyExitDistance(earlyExitDistance);
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Physics");
        ImGui::Separator();
        
        bool& gravity = m_Engine.GetGravity();
        ImGui::Checkbox("Gravity Enabled", &gravity);
        
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Camera");
        ImGui::Separator();
        
        ImGui::Text("Position: (%.2e, %.2e, %.2e)", 
                   m_Engine.GetCamera().GetOrbitalPosition().x, 
                   m_Engine.GetCamera().GetOrbitalPosition().y, 
                   m_Engine.GetCamera().GetOrbitalPosition().z);
        ImGui::Text("Radius: %.2e", m_Engine.GetCamera().GetOrbitalRadius());
        ImGui::Text("Azimuth: %.2f", m_Engine.GetCamera().GetAzimuth());
        ImGui::Text("Elevation: %.2f", m_Engine.GetCamera().GetElevation());
        
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

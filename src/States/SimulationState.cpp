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
        if (!m_Initialized) return;
        
        RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        RenderCommand::Clear();
        
        m_Engine.GenerateGrid(m_Engine.GetObjects());
        
        glm::mat4 view = glm::lookAt(m_Engine.GetCamera().GetOrbitalPosition(), m_Engine.GetCamera().GetTarget(), glm::vec3(0,1,0));
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(m_Engine.GetWidth())/m_Engine.GetHeight(), 1e9f, 1e14f);
        glm::mat4 viewProj = proj * view;
        
        m_Engine.DrawGrid(viewProj);
        
        RenderCommand::SetViewport(0, 0, m_Engine.GetWidth(), m_Engine.GetHeight());
        m_Engine.DispatchCompute(m_Engine.GetCamera());
        m_Engine.DrawFullScreenQuad();
    }
    
    void SimulationState::OnEvent(Event& event)
    {
        if (!m_Initialized) return;
        
        if (event.GetEventType() == EventType::MouseButtonPressed)
        {
            MouseButtonPressedEvent& e = (MouseButtonPressedEvent&)event;
            int button = e.GetMouseButton();
            
            m_Engine.GetCamera().ProcessOrbitalMouseButton(button, GLFW_PRESS, 0);
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
                m_Engine.SetGravity(!m_Engine.GetGravity());
                DONUT_INFO("Gravity turned {}", m_Engine.GetGravity() ? "ON" : "OFF");
            }
        }
    }
    
    void SimulationState::OnImUIRender()
    {
        ImGui::Begin("Simulation Controls");
        
        ImGui::Text("Black Hole Simulation");
        if (ImGui::Button("Back to Config"))
            Application::Get().GetStateManager().SwitchToState("Config");

        ImGui::Separator();
        
        ImGui::Text("Simulation Info:");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("Resolution: %dx%d", m_Engine.GetWidth(), m_Engine.GetHeight());
        ImGui::Text("Compute Resolution: %dx%d", m_Engine.GetComputeWidth(), m_Engine.GetComputeHeight());
        ImGui::Text("Objects: %zu", m_Engine.GetObjects().size());
        
        ImGui::Separator();
        
        ImGui::Text("Controls:");
        ImGui::Text("Left Mouse: Orbit camera");
        ImGui::Text("Scroll: Zoom in/out");
        ImGui::Text("G: Toggle gravity");
        ImGui::Text("Right Mouse: Enable gravity (hold)");
        
        ImGui::Separator();
        
        ImGui::Text("Physics:");
        bool gravity = m_Engine.GetGravity();
        if (ImGui::Checkbox("Gravity Enabled", &gravity))
            m_Engine.SetGravity(gravity);
        
        ImGui::Text("Camera:");
        ImGui::Text("Position: (%.2e, %.2e, %.2e)", 
                   m_Engine.GetCamera().GetOrbitalPosition().x, 
                   m_Engine.GetCamera().GetOrbitalPosition().y, 
                   m_Engine.GetCamera().GetOrbitalPosition().z);
        ImGui::Text("Radius: %.2e", m_Engine.GetCamera().GetRadius());
        ImGui::Text("Azimuth: %.2f", m_Engine.GetCamera().GetAzimuth());
        ImGui::Text("Elevation: %.2f", m_Engine.GetCamera().GetElevation());
        
        ImGui::End();
    }
};

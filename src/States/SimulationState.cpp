#include "SimulationState.h"
#include "Rendering/Renderer.h"
#include "Core/Application.h"

#include <imgui.h>

namespace Donut
{
    void SimulationState::OnEnter()
    {
        DONUT_INFO("Entering Simulation State");
    }
    
    void SimulationState::OnExit()
    {
        DONUT_INFO("Exiting Simulation State");
    }
    
    void SimulationState::OnUpdate(float deltaTime)
    {
    }
    
    void SimulationState::OnRender()
    {
        Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        Renderer::Clear();
    }
    
    void SimulationState::OnEvent(Event& event)
    {
    }
    
    void SimulationState::OnImUIRender()
    {
        ImGui::Begin("Simulation Controls");
        
        ImGui::Text("Simulation State");
        if (ImGui::Button("Back to Config"))
            Application::Get().GetStateManager().SwitchToState("Config");

        ImGui::Separator();
        
        ImGui::Text("Simulation Info:");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        
        ImGui::End();
    }
};

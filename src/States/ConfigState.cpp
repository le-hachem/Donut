#include "ConfigState.h"
#include "Rendering/Renderer.h"

namespace Donut
{
    void ConfigState::OnEnter()
    {
        DONUT_INFO("Entering Config State");
    }
    
    void ConfigState::OnExit()
    {
        DONUT_INFO("Exiting Config State");
    }
    
    void ConfigState::OnUpdate(float deltaTime)
    {
    }
    
    void ConfigState::OnRender()
    {
        Renderer::SetClearColor({ 0.1f, 0.1f, 0.2f, 1.0f });
        Renderer::Clear();
    }
    
    void ConfigState::OnEvent(Event& event)
    {
    }
}

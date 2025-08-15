#pragma once

#include "Core/State.h"
#include "Core/Event.h"
#include "Core/Log.h"
#include "Engine/Engine.h"
#include <vector>

namespace Donut
{
    class SimulationState
        : public State
    {
    public:
        ~SimulationState() = default;
        
        void OnEnter()                 override;
        void OnExit()                  override;
        void OnUpdate(float deltaTime) override;
        void OnRender()                override;
        void OnImUIRender()            override;
        void OnEvent(Event& event)     override;
    
    private:
        Engine m_Engine;
        bool m_Initialized = false;
    };
};

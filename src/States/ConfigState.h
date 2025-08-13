#pragma once

#include "Core/State.h"
#include "Core/Event.h"
#include "Core/Log.h"

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
    };
};
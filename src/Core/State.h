#pragma once

#include "Event.h"

namespace Donut
{
    class State
    {
    public:
        virtual ~State() = default;
        
        virtual void OnEnter()                 = 0;
        virtual void OnExit()                  = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnRender()                = 0;
        virtual void OnEvent(Event& event)     = 0;
    };
};
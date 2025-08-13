#pragma once

#include "Memory.h"
#include "State.h"

#include <string>
#include <unordered_map>

namespace Donut
{
    class StateManager
    {
    public:
        ~StateManager() = default;
        void Shutdown();
        
        void Update(float deltaTime);
        void Render();
        void OnImUIRender();
        void OnEvent(Event& event);
        
        void RegisterState(const std::string& stateName, Scope<State> state);
        void SwitchToState(const std::string& stateName);

        std::string GetCurrentStateName() const { return m_CurrentStateName; }
        State* GetCurrentState()          const { return m_CurrentState; }
        
        State* GetState(const std::string& stateName);
        bool   HasState(const std::string& stateName) const;
    private:
        void CreateStates();
        void DestroyStates();

    private:
        State*      m_CurrentState     = nullptr;
        std::string m_CurrentStateName = "";
        
        std::unordered_map<std::string, Scope<State>> m_States;
    };
}

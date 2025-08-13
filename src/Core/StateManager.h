#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "State.h"

namespace Donut
{
    class StateManager
    {
    public:
        ~StateManager() = default;
        void Shutdown();
        
        void Update(float deltaTime);
        void Render();
        void OnEvent(Event& event);
        
        void RegisterState(const std::string& stateName, std::unique_ptr<State> state);
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
        
        std::unordered_map<std::string, std::unique_ptr<State>> m_States;
    };
}

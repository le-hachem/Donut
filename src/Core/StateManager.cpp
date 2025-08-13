#include "StateManager.h"
#include "Log.h"

namespace Donut
{
    void StateManager::Shutdown()
    {
        if (m_CurrentState)
            m_CurrentState->OnExit();
            
        DestroyStates();
        DONUT_INFO("StateManager shutdown");
    }

    void StateManager::DestroyStates()
    {
        m_States.clear();
        m_CurrentState = nullptr;
        m_CurrentStateName = "";
    }

    void StateManager::RegisterState(const std::string& stateName, Scope<State> state)
    {
        if (m_States.find(stateName) != m_States.end())
            DONUT_WARN("State '{}' already exists, overwriting", stateName);
        m_States[stateName] = std::move(state);
        DONUT_INFO("Registered state: {}", stateName);
    }

    void StateManager::SwitchToState(const std::string& stateName)
    {
        auto it = m_States.find(stateName);
        if (it == m_States.end())
        {
            DONUT_ERROR("Attempted to switch to unknown state: {}", stateName);
            return;
        }

        State* newState = it->second.get();
        if (newState == m_CurrentState)
            return;

        if (m_CurrentState)
            m_CurrentState->OnExit();

        m_CurrentState = newState;
        m_CurrentStateName = stateName;
        m_CurrentState->OnEnter();
        DONUT_INFO("Switched to state: {}", stateName);
    }

    State* StateManager::GetState(const std::string& stateName)
    {
        auto it = m_States.find(stateName);
        if (it != m_States.end())
            return it->second.get();
        return nullptr;
    }

    bool StateManager::HasState(const std::string& stateName) const
    {
        return m_States.find(stateName) != m_States.end();
    }

    void StateManager::Update(float deltaTime)
    {
        if (m_CurrentState)
            m_CurrentState->OnUpdate(deltaTime);
    }

    void StateManager::Render()
    {
        if (m_CurrentState)
            m_CurrentState->OnRender();
    }

    void StateManager::OnEvent(Event& event)
    {
        if (m_CurrentState)
            m_CurrentState->OnEvent(event);
    }
}

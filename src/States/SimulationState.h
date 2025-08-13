#pragma once

#include "Core/State.h"
#include "Core/Event.h"
#include "Core/Camera.h"
#include "Core/Log.h"

#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

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
        void OnEvent(Event& event)     override;
    private:
        void HandleKeyInput(float deltaTime);
        void UpdateTexture();
    private:
        std::unique_ptr<Camera> m_Camera;
        
        std::shared_ptr<VertexArray>  m_VertexArray;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer>  m_IndexBuffer;
        std::shared_ptr<Shader>       m_Shader;
        std::shared_ptr<Texture2D>    m_Texture;
        
        std::shared_ptr<Shader>    m_ComputeShader;
        std::shared_ptr<Texture2D> m_ProcessedTexture;
        
        float m_ComputeBrightness = 0.0f;
        float m_ComputeContrast   = 1.0f;
        float m_ComputeSaturation = 1.0f;
        bool  m_UseComputeShader  = true;
        
        bool  m_Keys[1024]    = { false };
        bool  m_FirstMouse    = true;
        float m_LastX         = 0.0f;
        float m_LastY         = 0.0f;
        float m_DeltaTime     = 0.0f;
        float m_LastFrame     = 0.0f;
        bool  m_MouseDragging = false;
        float m_TextureTime   = 0.0f;
    };
};

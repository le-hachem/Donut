#pragma once

#include "Core/Memory.h"
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
        Scope<Camera> m_Camera;
        
        Ref<VertexArray>  m_VertexArray;
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer>  m_IndexBuffer;
        Ref<Shader>       m_Shader;
        Ref<Texture2D>    m_Texture;
        
        Ref<Shader>    m_ComputeShader;
        Ref<Texture2D> m_ProcessedTexture;
        
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

#pragma once

#include "Core/Camera.h"
#include "Core/State.h"
#include "Core/Event.h"
#include "Core/Log.h"

#include "Engine/Scene.h"
#include "Engine/Object.h"

#include "Rendering/Renderer.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexArray.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <vector>
#include <memory>

namespace Donut
{
    class WorldBuilderState
        : public State
    {
    public:
        ~WorldBuilderState() = default;
        
        void OnEnter()                 override;
        void OnExit()                  override;
        void OnUpdate(float deltaTime) override;
        void OnRender()                override;
        void OnImUIRender()            override;
        void OnEvent(Event& event)     override;
    private:
        void AddSphere();
        void RemoveSelectedObject();
        void ClearScene();
        void SaveScene();
        void LoadScene();
        void RenderScene();
        void InitializeSphereGeometry();
        void InitializeGridGeometry();
        void RenderGrid();
    private:
        Scene  m_Scene;
        Camera m_Camera;
        bool   m_Initialized = false;
        
        Ref<Shader>      m_SphereShader;
        Ref<VertexArray> m_SphereVAO;
        Ref<CubemapTexture> m_HDRIEnvironment;
        
        Ref<Shader>      m_SkyboxShader;
        Ref<VertexArray> m_SkyboxVAO;
        
        Ref<Shader>      m_GridShader;
        Ref<VertexArray> m_GridVAO;
        
        glm::vec3 m_NewObjectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        float     m_NewObjectRadius   = 1.0f;
        glm::vec3 m_NewObjectColor    = glm::vec3(1.0f, 1.0f, 1.0f);
        float     m_NewObjectSpecular = 0.5f;
        float     m_NewObjectEmission = 0.0f;
        
        int       m_SelectedObjectIndex = -1;
        bool      m_CameraDragging      = false;
        glm::vec2 m_LastMousePos        = glm::vec2(0.0f, 0.0f);
        
        ImGuizmo::OPERATION m_GizmoOperation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE      m_GizmoMode      = ImGuizmo::LOCAL;
        
        bool m_ShowObjectList      = true;
        bool m_ShowObjectCreator   = true;
        bool m_ShowSceneInfo       = true;
        bool m_ShowGizmoControls   = true;
        bool m_ShowOutlineControls = true;
        bool m_ShowGrid            = true;

        glm::vec3 m_OutlineColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float     m_OutlineWidth = 0.25f;
        
        glm::vec3 m_GridColor = glm::vec3(0.5f, 0.5f, 0.5f);
        float     m_GridAlpha = 0.5f;
        float     m_GridSize  = 50.0f;
        
        Object m_BlackHole;
        bool   m_BlackHoleInitialized = false;
        
        void SetHDRIEnvironment(Ref<CubemapTexture> hdri) { m_HDRIEnvironment = hdri; }
        Ref<CubemapTexture> GetHDRIEnvironment()    const { return m_HDRIEnvironment; }
        
        void InitializeSkyboxGeometry();
        void RenderSkybox();
    };
};

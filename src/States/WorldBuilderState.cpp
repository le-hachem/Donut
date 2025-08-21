#include "WorldBuilderState.h"

#include "Core/Application.h"
#include "Core/Window.h"

#include "Rendering/Renderer.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Texture.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <nlohmann/json.hpp>

#include <numbers>
#include <fstream>
#include <sstream>
#include <limits>

namespace Donut
{
    void WorldBuilderState::OnEnter()
    {
        DONUT_INFO("Entering World Builder State");
        
        ImGuizmo::Enable(true);

        m_Camera.SetCameraMode(CameraMode::Orbital);
        m_Camera.SetOrbitalTarget(glm::vec3(0.0f, 0.0f, 0.0f));
        m_Camera.SetOrbitalRadius(15.0f);
        m_Camera.SetOrbitalLimits(2.0f, 200.0f);
        m_Camera.SetOrbitalSpeed(0.01f);
        m_Camera.SetZoomSpeed(2.0f);
        m_Camera.SetAzimuth(0.0f);
        m_Camera.SetElevation(static_cast<float>(std::numbers::pi) / 3.0f);
        m_Camera.UpdateOrbital();
        
        // Initialize rendering resources once
        m_SphereShader = Ref<Shader>(Shader::Create("Assets/Shaders/Sphere.glsl"));
        InitializeSphereGeometry();
        
        // Initialize the black hole at the center
        Material blackHoleMaterial(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f); // Pure black, no specular, no emission
        m_BlackHole = Object(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f, blackHoleMaterial); // Position at origin, radius 2
        m_BlackHoleInitialized = true;
        
        m_Initialized = true;
    }
    
    void WorldBuilderState::OnExit()
    {
        DONUT_INFO("Exiting World Builder State");
    }
    
    void WorldBuilderState::OnUpdate(float deltaTime)
    {
        if (m_CameraDragging && 
            !ImGuizmo::IsUsing())
        {
            GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            
            glm::vec2 currentMousePos(xpos, ypos);
            glm::vec2 delta = currentMousePos - m_LastMousePos;
            
            float sensitivity    = 0.005f;
            float azimuthDelta   = delta.x * sensitivity;
            float elevationDelta = -delta.y * sensitivity;
            
            float newAzimuth   = m_Camera.GetAzimuth() + azimuthDelta;
            float newElevation = m_Camera.GetElevation() + elevationDelta;
            
            newElevation = glm::clamp(newElevation, 0.01f, static_cast<float>(std::numbers::pi) - 0.01f);
            
            m_Camera.SetAzimuth(newAzimuth);
            m_Camera.SetElevation(newElevation);
            m_Camera.UpdateOrbital();
            
            m_LastMousePos = currentMousePos;
        }
    }
    
    void WorldBuilderState::OnRender()
    {
        RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        RenderCommand::Clear();
        RenderScene();
    }
    
    void WorldBuilderState::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) 
        {
            if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
            {
                if (ImGuizmo::IsUsing() || 
                    ImGuizmo::IsOver())
                    return false;
                
                GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                
                float ndcX = (2.0f * static_cast<float>(xpos)) / static_cast<float>(width) - 1.0f;
                float ndcY = 1.0f - (2.0f * static_cast<float>(ypos)) / static_cast<float>(height);
                
                glm::vec4 rayStart_NDC(ndcX, ndcY, -1.0f, 1.0f);
                glm::vec4 rayEnd_NDC(ndcX, ndcY, 0.0f, 1.0f);
                
                glm::mat4 invVP          = glm::inverse(m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix());
                glm::vec4 rayStart_World = invVP * rayStart_NDC;
                glm::vec4 rayEnd_World   = invVP * rayEnd_NDC;
                
                rayStart_World /= rayStart_World.w;
                rayEnd_World   /= rayEnd_World.w;
                
                glm::vec3 rayDir    = glm::normalize(glm::vec3(rayEnd_World - rayStart_World));
                glm::vec3 rayOrigin = glm::vec3(rayStart_World);
                
                float closestDistance  = std::numeric_limits<float>::max();
                int closestObjectIndex = -1;
                
                if (m_BlackHoleInitialized)
                {
                    glm::vec3 oc       = rayOrigin - m_BlackHole.m_Centre;
                    float a            = glm::dot(rayDir, rayDir);
                    float b            = 2.0f * glm::dot(oc, rayDir);
                    float c            = glm::dot(oc, oc) - m_BlackHole.m_Radius * m_BlackHole.m_Radius;
                    float discriminant = b * b - 4 * a * c;
                    
                    if (discriminant > 0)
                    {
                        float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
                        float t2 = (-b + sqrt(discriminant)) / (2.0f * a);
                        
                        if (t1 > 0 && t1 < closestDistance)
                        {
                            closestDistance = t1;
                            closestObjectIndex = -2;
                        }
                        else if (t2 > 0 && t2 < closestDistance)
                        {
                            closestDistance = t2;
                            closestObjectIndex = -2;
                        }
                    }
                }
                
                for (size_t i = 0; i < m_Scene.objs.size(); ++i)
                {
                    const Object& obj = m_Scene.objs[i];
                    
                    glm::vec3 oc       = rayOrigin - obj.m_Centre;
                    float a            = glm::dot(rayDir, rayDir);
                    float b            = 2.0f * glm::dot(oc, rayDir);
                    float c            = glm::dot(oc, oc) - obj.m_Radius * obj.m_Radius;
                    float discriminant = b * b - 4 * a * c;
                    
                    if (discriminant > 0)
                    {
                        float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
                        float t2 = (-b + sqrt(discriminant)) / (2.0f * a);
                        
                        if (t1 > 0 && t1 < closestDistance)
                        {
                            closestDistance = t1;
                            closestObjectIndex = static_cast<int>(i);
                        }
                        else if (t2 > 0 && t2 < closestDistance)
                        {
                            closestDistance = t2;
                            closestObjectIndex = static_cast<int>(i);
                        }
                    }
                }
                
                if (closestObjectIndex >= 0)
                {
                    m_SelectedObjectIndex = closestObjectIndex;
                    DONUT_INFO("Selected object {}", closestObjectIndex);
                    return true;
                }
                else if (closestObjectIndex == -2)
                {
                    m_SelectedObjectIndex = -1;
                    DONUT_INFO("Black hole clicked (not selectable)");
                    return true;
                }
                else
                {
                    m_SelectedObjectIndex = -1;
                    m_CameraDragging      = true;
                    m_LastMousePos        = glm::vec2(xpos, ypos);
                }
                
                return true;
            }
            return false;
        });
        
        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e) 
        {
            if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
            {
                if (!ImGuizmo::IsUsing())
                    m_CameraDragging = false;
                return true;
            }

            return false;
        });
        
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
        {
            int newWidth = e.GetWidth();
            int newHeight = e.GetHeight();
            if (newWidth > 0 && newHeight > 0)
            {
                float aspect = static_cast<float>(newWidth) / static_cast<float>(newHeight);
                m_Camera.SetProjection(45.0f, aspect, 0.1f, 1000.0f);
            }
            return false;
        });
        
        dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& e) 
        {
            float zoomSpeed = 0.1f;
            float zoomDelta = e.GetYOffset() * zoomSpeed;
            
            double currentRadius = m_Camera.GetOrbitalRadius();
            double newRadius = currentRadius - zoomDelta * currentRadius * 0.1f;
            
            double minRadius = 2.0f;
            double maxRadius = 200.0f;
            newRadius        = glm::clamp(newRadius, minRadius, maxRadius);
            
            m_Camera.SetOrbitalRadius(newRadius);
            m_Camera.UpdateOrbital();

            return true;
        });
        
        dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) 
        {
            if (m_SelectedObjectIndex >= 0 && 
                m_SelectedObjectIndex < static_cast<int>(m_Scene.objs.size()))
            {
                switch (e.GetKeyCode())
                {
                    case GLFW_KEY_W:
                        m_GizmoOperation = ImGuizmo::TRANSLATE;
                        return true;
                    case GLFW_KEY_E:
                        m_GizmoOperation = ImGuizmo::ROTATE;
                        return true;
                    case GLFW_KEY_R:
                        m_GizmoOperation = ImGuizmo::SCALE;
                        return true;
                }
            }
            return false;
        });
    }
    
    void WorldBuilderState::OnImUIRender()
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
     
        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGuizmo::SetRect(vp->Pos.x, vp->Pos.y, vp->Size.x, vp->Size.y);
        
        ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("World Builder", nullptr, ImGuiWindowFlags_NoCollapse);
        
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "World Builder");
        ImGui::PopFont();
        ImGui::Separator();
        
                 if (ImGui::CollapsingHeader("Scene Info", &m_ShowSceneInfo))
         {
            ImGui::Text("Objects in scene: %zu/16 (+ 1 black hole)", m_Scene.objs.size());
            
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Camera Info:");
            
            glm::vec3 cameraPos = m_Camera.GetOrbitalPosition();
            glm::vec3 target = m_Camera.GetOrbitalTarget();
            
            ImGui::Text("Position: (%.2f, %.2f, %.2f)", cameraPos.x, cameraPos.y, cameraPos.z);
            ImGui::Text("Target: (%.2f, %.2f, %.2f)", target.x, target.y, target.z);
            ImGui::Text("Distance: %.2f", m_Camera.GetOrbitalRadius());
            ImGui::Text("Azimuth: %.1f°", glm::degrees(m_Camera.GetAzimuth()));
            ImGui::Text("Elevation: %.1f°", glm::degrees(m_Camera.GetElevation()));
            
            ImGui::Separator();
            
            if (ImGui::Button("Reset Camera"))
            {
                m_Camera.SetOrbitalRadius(15.0f);
                m_Camera.SetAzimuth(0.0f);
                m_Camera.SetElevation(static_cast<float>(std::numbers::pi) / 3.0f);
                m_Camera.SetOrbitalTarget(glm::vec3(0.0f, 0.0f, 0.0f));
                m_Camera.UpdateOrbital();
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Clear Scene"))
                ClearScene();
            
            ImGui::SameLine();
            if (ImGui::Button("Focus on Objects"))
            {
                if (!m_Scene.objs.empty())
                {
                    glm::vec3 center(0.0f);
                    for (const auto& obj : m_Scene.objs)
                        center += obj.m_Centre;
                    center /= static_cast<float>(m_Scene.objs.size());
                    m_Camera.SetOrbitalTarget(center);
                    m_Camera.UpdateOrbital();
                }
            }
        }
        
        ImGui::Spacing();
        
        if (ImGui::CollapsingHeader("Create Object", &m_ShowObjectCreator))
        {
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "New Sphere");
            ImGui::Separator();
            
            // Show object count and limit
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Objects: %zu/16", m_Scene.objs.size());
            
            ImGui::Text("Position:");
            ImGui::DragFloat3("##Position", &m_NewObjectPosition.x, 0.1f);
            
            ImGui::Text("Radius:");
            ImGui::DragFloat("##Radius", &m_NewObjectRadius, 0.1f, 0.1f, 10.0f);
            
            ImGui::Text("Color:");
            ImGui::ColorEdit3("##Color", &m_NewObjectColor.x);
            
            ImGui::Text("Specular:");
            ImGui::SliderFloat("##Specular", &m_NewObjectSpecular, 0.0f, 1.0f);
            
            ImGui::Text("Emission:");
            ImGui::SliderFloat("##Emission", &m_NewObjectEmission, 0.0f, 1.0f);
            
            ImGui::Spacing();
            
            // Disable button if object limit reached
            if (m_Scene.objs.size() >= 16)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                ImGui::Button("Add Sphere (Limit Reached)", ImVec2(ImGui::GetWindowWidth() - 20, 30));
                ImGui::PopStyleVar();
            }
            else
            {
                if (ImGui::Button("Add Sphere", ImVec2(ImGui::GetWindowWidth() - 20, 30)))
                    AddSphere();
            }
        }
        
        ImGui::Spacing();
        
        if (ImGui::CollapsingHeader("Scene Objects", &m_ShowObjectList))
        {
            if (m_BlackHoleInitialized)
            {
                ImGui::PushID(-1);
                
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Black Hole (Center)");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Position: (0.00, 0.00, 0.00)");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Radius: %.2f", m_BlackHole.m_Radius);
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Color: Black (with white outline)");
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Cannot be removed or modified");
                
                ImGui::PopID();
                ImGui::Separator();
            }
            
            if (m_Scene.objs.empty())
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No additional objects in scene");
            else
            {
                for (size_t i = 0; i < m_Scene.objs.size(); ++i)
                {
                    Object& obj = m_Scene.objs[i];
                    
                    ImGui::PushID(static_cast<int>(i));
                    
                    bool isSelected = (m_SelectedObjectIndex == static_cast<int>(i));
                    if (ImGui::Selectable(("Sphere " + std::to_string(i)).c_str(), isSelected))
                        m_SelectedObjectIndex = static_cast<int>(i);
                    
                    if (isSelected)
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Remove"))
                        {
                            RemoveSelectedObject();
                        }
                        
                        ImGui::Text("Position: (%.2f, %.2f, %.2f)", 
                                   obj.m_Centre.x, obj.m_Centre.y, obj.m_Centre.z);
                        ImGui::Text("Radius: %.2f", obj.m_Radius);
                        ImGui::Text("Color: (%.2f, %.2f, %.2f)", 
                                   obj.m_Material.m_Color.x, obj.m_Material.m_Color.y, obj.m_Material.m_Color.z);
                    }
                    
                    ImGui::PopID();
                }
            }
        }

        ImGui::Spacing();
         
        if (ImGui::CollapsingHeader("Gizmo Controls", &m_ShowGizmoControls))
        {
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Gizmo Operation:");
            
            if (ImGui::RadioButton("Translate", m_GizmoOperation == ImGuizmo::TRANSLATE))
                m_GizmoOperation = ImGuizmo::TRANSLATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", m_GizmoOperation == ImGuizmo::ROTATE))
                m_GizmoOperation = ImGuizmo::ROTATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", m_GizmoOperation == ImGuizmo::SCALE))
                m_GizmoOperation = ImGuizmo::SCALE;
            
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Hotkeys:");
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "W = Translate, E = Rotate, R = Scale");
        }

        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Selection Outline", &m_ShowOutlineControls))
        {
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Outline Settings:");

            ImGui::Text("Outline Color:");
            ImGui::ColorEdit3("##OutlineColor", &m_OutlineColor.x);

            ImGui::Text("Outline Width:");
            ImGui::SliderFloat("##OutlineWidth", &m_OutlineWidth, 0.01f, 0.9f, "%.2f");

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "White rim around sphere silhouette");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        float buttonWidth = (ImGui::GetWindowWidth() - 30) / 2.0f;
        
        if (ImGui::Button("Save Scene", ImVec2(buttonWidth, 30)))
            SaveScene();
        
        ImGui::SameLine();
        if (ImGui::Button("Load Scene", ImVec2(buttonWidth, 30)))
            LoadScene();
        
        ImGui::Spacing();
        
        if (ImGui::Button("Back to Config", ImVec2(ImGui::GetWindowWidth() - 20, 30)))
            Application::Get().GetStateManager().SwitchToState("Config");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Camera: Left mouse = rotate, Scroll = zoom");
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Gizmo: W=Translate, E=Rotate, R=Scale");
        
        ImGui::End();
        
        if (m_SelectedObjectIndex >= 0 && 
            m_SelectedObjectIndex < static_cast<int>(m_Scene.objs.size()) &&
            m_SelectedObjectIndex != -2)
        {
            glm::mat4 view       = m_Camera.GetViewMatrix();
            glm::mat4 projection = m_Camera.GetProjectionMatrix();
            
            Object& selectedObj = m_Scene.objs[m_SelectedObjectIndex];
            
            glm::mat4 objectTransform = glm::mat4(1.0f);
                      objectTransform = glm::translate(objectTransform, selectedObj.m_Centre);
                      objectTransform = glm::scale(objectTransform, glm::vec3(selectedObj.m_Radius));
            
            DONUT_INFO("Object position: ({}, {}, {})", selectedObj.m_Centre.x, selectedObj.m_Centre.y, selectedObj.m_Centre.z);
            glm::vec3 cameraPos = m_Camera.GetOrbitalPosition();
            DONUT_INFO("Camera position: ({}, {}, {})", cameraPos.x, cameraPos.y, cameraPos.z);
            
            glm::vec3 viewTranslation = glm::vec3(view[3][0], view[3][1], view[3][2]);
            DONUT_INFO("View translation: ({}, {}, {})", viewTranslation.x, viewTranslation.y, viewTranslation.z);
            
            ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
            if (ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), 
                                   m_GizmoOperation, m_GizmoMode, 
                                   glm::value_ptr(objectTransform), 
                                   nullptr, nullptr))
            {
                glm::vec3 newPosition = glm::vec3(objectTransform[3][0], objectTransform[3][1], objectTransform[3][2]);
                selectedObj.m_Centre = newPosition;
                
                glm::vec3 scale = glm::vec3
                (
                    glm::length(glm::vec3(objectTransform[0])),
                    glm::length(glm::vec3(objectTransform[1])),
                    glm::length(glm::vec3(objectTransform[2]))
                );
                selectedObj.m_Radius = (scale.x + scale.y + scale.z) / 3.0f;
                
                DONUT_INFO("Matrix [3]: ({}, {}, {}, {})", objectTransform[3][0], objectTransform[3][1], objectTransform[3][2], objectTransform[3][3]);
                DONUT_INFO("New position: ({}, {}, {})", newPosition.x, newPosition.y, newPosition.z);
            }
        }
    }
    
    void WorldBuilderState::AddSphere()
    {
        // Check if we've reached the object limit (16 objects)
        if (m_Scene.objs.size() >= 16)
        {
            DONUT_WARN("Cannot add more objects. Maximum of 16 objects reached.");
            return;
        }
        
        Material material(m_NewObjectColor, m_NewObjectSpecular, m_NewObjectEmission);
        Object   sphere(m_NewObjectPosition, m_NewObjectRadius, material);
        m_Scene.objs.push_back(sphere);
        
        // Automatically select the newly created object
        m_SelectedObjectIndex = static_cast<int>(m_Scene.objs.size() - 1);
        
        m_NewObjectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        m_NewObjectRadius   = 1.0f;
        m_NewObjectColor    = glm::vec3(1.0f, 1.0f, 1.0f);
        m_NewObjectSpecular = 0.5f;
        m_NewObjectEmission = 0.0f;
        
        DONUT_INFO("Added sphere to scene and selected it");
    }
    
    void WorldBuilderState::RemoveSelectedObject()
    {
        if (m_SelectedObjectIndex >= 0 && m_SelectedObjectIndex < static_cast<int>(m_Scene.objs.size()))
        {
            m_Scene.objs.erase(m_Scene.objs.begin() + m_SelectedObjectIndex);
            m_SelectedObjectIndex = -1;
            DONUT_INFO("Removed object from scene");
        }
    }
    
    void WorldBuilderState::ClearScene()
    {
        m_Scene.objs.clear();
        m_SelectedObjectIndex = -1;
        DONUT_INFO("Cleared scene (black hole remains at center)");
    }
    
    void WorldBuilderState::SaveScene()
    {
        nlohmann::json sceneData;
        sceneData["objects"] = nlohmann::json::array();

        for (const auto& obj : m_Scene.objs)
        {
            nlohmann::json sphereData;
            sphereData["position"] = 
            { 
                obj.m_Centre.x, 
                obj.m_Centre.y, 
                obj.m_Centre.z 
            };

            sphereData["color"] = 
            {
                obj.m_Material.m_Color.x, 
                obj.m_Material.m_Color.y, 
                obj.m_Material.m_Color.z
            };

            sphereData["radius"]  = obj.m_Radius;
            sphereData["specular"] = obj.m_Material.m_Specular;
            sphereData["emission"] = obj.m_Material.m_Emission;
            sceneData["objects"].push_back(sphereData);
        }

        std::ofstream file("Scene.json");
        if (file.is_open())
        {
            file << sceneData.dump(4);
            file.close();
            DONUT_INFO("Scene saved to Scene.json (black hole always present at center)");
        }
        else
            DONUT_ERROR("Failed to save scene");
    }
    
    void WorldBuilderState::LoadScene()
    {
        std::ifstream file("Scene.json");
        if (file.is_open())
        {
            m_Scene.objs.clear();
            m_SelectedObjectIndex = -1;
            
            nlohmann::json sceneData;
            file >> sceneData;

            if (sceneData.contains("objects"))
            {
                for (const auto& sphereData : sceneData["objects"])
                {
                    glm::vec3 position;
                    glm::vec3 color;
                    float radius;
                    float specular;
                    float emission;
                    
                    auto posArray = sphereData["position"];
                    position.x = posArray[0].get<float>();
                    position.y = posArray[1].get<float>();
                    position.z = posArray[2].get<float>();
                    
                    auto colorArray = sphereData["color"];
                    color.x = colorArray[0].get<float>();
                    color.y = colorArray[1].get<float>();
                    color.z = colorArray[2].get<float>();
                    
                    radius   = sphereData["radius"].get<float>();
                    specular = sphereData["specular"].get<float>();
                    emission = sphereData["emission"].get<float>();
                    
                    Material material(color, specular, emission);
                    Object   sphere(position, radius, material);
                    m_Scene.objs.push_back(sphere);
                }
            }
            
            file.close();
            DONUT_INFO("Scene loaded from Scene.json (black hole remains at center)");
        }
        else
            DONUT_ERROR("Failed to load scene");
    }
    
    void WorldBuilderState::InitializeSphereGeometry()
    {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        
        const int segments = 32;
        const int rings = 16;
        
        for (int ring = 0; ring <= rings; ++ring)
        {
            float phi = static_cast<float>(std::numbers::pi) * ring / rings;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            
            for (int segment = 0; segment <= segments; ++segment)
            {
                float theta = 2.0f * static_cast<float>(std::numbers::pi) * segment / segments;
                float sinTheta = sin(theta);
                float cosTheta = cos(theta);
                
                float x = cosTheta * sinPhi;
                float y = cosPhi;
                float z = sinTheta * sinPhi;
                
                float nx = x;
                float ny = y;
                float nz = z;
                
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
            }
        }
        
        for (int ring = 0; ring < rings; ++ring)
        {
            for (int segment = 0; segment < segments; ++segment)
            {
                uint32_t first = ring * (segments + 1) + segment;
                uint32_t second = first + segments + 1;
                
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);
                
                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
        
        auto vertexBuffer = Ref<VertexBuffer>(VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(float))));
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        vertexBuffer->SetLayout(layout);
        
        auto indexBuffer = Ref<IndexBuffer>(IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size())));
        
        m_SphereVAO = Ref<VertexArray>(VertexArray::Create());
        m_SphereVAO->AddVertexBuffer(vertexBuffer);
        m_SphereVAO->SetIndexBuffer(indexBuffer);
    }
    
    void WorldBuilderState::RenderScene()
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        RenderCommand::SetViewport(0, 0, width, height);
        RenderCommand::EnableDepthTest();
        
        glm::mat4 view           = m_Camera.GetViewMatrix();
        glm::mat4 projection     = m_Camera.GetProjectionMatrix();
        glm::mat4 viewProjection = projection * view;
        
        glm::vec3 lightPos  = m_Scene.m_LightPos;
        glm::vec3 cameraPos = m_Camera.GetOrbitalPosition();
        
        m_SphereShader->Bind();
        m_SphereShader->SetMat4("u_ViewProjection", viewProjection);
        m_SphereShader->SetFloat3("u_LightPos",  lightPos);
        m_SphereShader->SetFloat3("u_CameraPos", cameraPos);

        m_SphereShader->SetFloat3("u_OutlineColor", m_OutlineColor);
        m_SphereShader->SetFloat("u_OutlineWidth",  m_OutlineWidth);
        
        if (m_BlackHoleInitialized)
        {
            glm::mat4 blackHoleTransform = glm::translate(glm::mat4(1.0f), m_BlackHole.m_Centre);
                      blackHoleTransform = glm::scale(blackHoleTransform, glm::vec3(m_BlackHole.m_Radius));
            
            m_SphereShader->SetMat4("u_Transform", blackHoleTransform);
            m_SphereShader->SetInt("u_IsSelected", 1);
            m_SphereShader->SetFloat3("u_Color", m_BlackHole.m_Material.m_Color);
            m_SphereShader->SetFloat("u_Emission", m_BlackHole.m_Material.m_Emission);
            m_SphereShader->SetFloat("u_Specular", m_BlackHole.m_Material.m_Specular);
            m_SphereShader->SetFloat("u_OutlineWidth", 0.3f);
            
            m_SphereVAO->Bind();
            RenderCommand::DrawIndexed(m_SphereVAO);
            
            m_SphereShader->SetFloat("u_OutlineWidth", m_OutlineWidth);
        }
        
        for (size_t i = 0; i < m_Scene.objs.size(); ++i)
        {
            const auto& obj = m_Scene.objs[i];
            
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), obj.m_Centre);
                      transform = glm::scale(transform, glm::vec3(obj.m_Radius));
            
            m_SphereShader->SetMat4("u_Transform", transform);
            
            bool isSelected = (m_SelectedObjectIndex == static_cast<int>(i));
            m_SphereShader->SetInt("u_IsSelected", isSelected ? 1 : 0);
            if (isSelected)
            {
                glm::vec3 highlightColor = obj.m_Material.m_Color * 1.5f;
                highlightColor = glm::clamp(highlightColor, 0.0f, 1.0f);
                m_SphereShader->SetFloat3("u_Color", highlightColor);
                m_SphereShader->SetFloat("u_Emission", 0.2f);
            }
            else
            {
                m_SphereShader->SetFloat3("u_Color", obj.m_Material.m_Color);
                m_SphereShader->SetFloat("u_Emission", obj.m_Material.m_Emission);
            }
            
            m_SphereShader->SetFloat("u_Specular", obj.m_Material.m_Specular);
            
            m_SphereVAO->Bind();
            RenderCommand::DrawIndexed(m_SphereVAO);
        }
        
        RenderCommand::DisableDepthTest();
    }
};

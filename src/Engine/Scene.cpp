#include "Scene.h"

namespace Donut
{
    Scene::Scene() 
        : m_LightPos(5.0f, 5.0f, 5.0f) { }

    glm::vec3 Scene::Trace(Ray &ray)
    {
        float closest = std::numeric_limits<float>::infinity();
        const Object* hitObj = nullptr;

        for(auto& obj : objs)
        {
            float t;

            if(obj.Intersect(ray, t))
                if(t < closest) 
                {
                    closest = t;
                    hitObj = &obj;
                }
        }
        
        if(hitObj)
        {
            glm::vec3 hitPoint = ray.m_Origin + ray.m_Direction * closest;
            glm::vec3 normal = hitObj->GetNormal(hitPoint);
            glm::vec3 lightDir = glm::normalize(m_LightPos - hitPoint);

            float diff = std::max(glm::dot(normal, lightDir), 0.0f);

            Ray shadowRay(hitPoint + normal * 0.001f, lightDir);
            bool inShadow = false;
            
            for(auto& obj : objs) 
            {
                float t;

                if(obj.Intersect(shadowRay, t)) 
                {
                    inShadow = true;
                    break;
                }
            }

            glm::vec3 color = hitObj->m_Material.m_Color;
            float ambient = 0.1f;

            if (inShadow) 
                return color * ambient;
            return color * (ambient + diff * 0.9f);
        }

        return glm::vec3(0.0f, 0.0f, 0.1f); 
    }
}

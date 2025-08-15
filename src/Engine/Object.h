#pragma once

#include <glm/glm.hpp>

namespace Donut
{
    class Ray
    {
    public:
        glm::vec3 m_Direction;
        glm::vec3 m_Origin;

        Ray(glm::vec3 o, glm::vec3 d) 
            : m_Origin(o), 
              m_Direction(glm::normalize(d)) { }
    };

    class Material
    {
    public:
        glm::vec3 m_Color;
        float     m_Specular;
        float     m_Emission;

        Material(glm::vec3 c, float s, float e)
            : m_Color(c), 
              m_Specular(s), 
              m_Emission(e) { }
    };

    class Object
    {
    public:
        glm::vec3 m_Centre;
        float     m_Radius;
        Material  m_Material;
    
        Object(glm::vec3 c, float r, Material m) 
            : m_Centre(c), 
              m_Radius(r), 
              m_Material(m) { }
    
        bool Intersect(Ray &ray, float &t)
        {
            glm::vec3 oc = ray.m_Origin - m_Centre;
            float a = glm::dot(ray.m_Direction, ray.m_Direction); 
            float b = 2.0f * glm::dot(oc, ray.m_Direction);
            float c = glm::dot(oc, oc) - m_Radius * m_Radius;
            double discriminant = b*b - 4*a*c;

            if(discriminant < 0)
                return false;
    
            float intercept = (-b - sqrt(discriminant)) / (2.0f*a);
            if(intercept < 0)
            {
                intercept = (-b + sqrt(discriminant)) / (2.0f*a);
                if(intercept<0)
                    return false;
            }

            t = intercept;
            return true;
        }
    
        glm::vec3 GetNormal(glm::vec3 &point) const
        {
            return glm::normalize(point - m_Centre);
        }
    };
};

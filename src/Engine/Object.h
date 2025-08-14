#pragma once

#include <glm/glm.hpp>

namespace Donut
{
    class Ray
    {
    public:
        glm::vec3 m_Direction;
        glm::vec3 m_Origin;

        Ray(glm::vec3 o, glm::vec3 d);
    };

    class Material
    {
    public:
        glm::vec3 m_Color;
        float     m_Specular;
        float     m_Emission;

        Material(glm::vec3 c, float s, float e);
    };

    class Object
    {
    public:
        glm::vec3 m_Centre;
        float     m_Radius;
        Material  m_Material;
    
        Object(glm::vec3 c, float r, Material m);
    
        bool Intersect(Ray &ray, float &t);
        glm::vec3 GetNormal(glm::vec3 &point) const;
    };
};

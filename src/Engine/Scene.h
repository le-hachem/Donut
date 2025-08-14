#pragma once

#include <vector>
#include <limits>
#include <glm/glm.hpp>

#include "Object.h"

namespace Donut
{
    class Scene 
    {
    public:
        std::vector<Object> objs;
        glm::vec3 m_LightPos;

        Scene();
        glm::vec3 Trace(Ray &ray);
    };
};
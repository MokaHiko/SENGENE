#ifndef RAY_H
#define RAY_H

#pragma once
#include <glm/glm.hpp>
#include "Body.h"

namespace flg {
    struct Ray
    {
        glm::vec3 Direction;
        Ray(const glm::vec3 origin, const glm::vec3 direction);
    };

    struct Raycasthit
    {
        Body* body;
        bool DidHit() { body != nullptr;};
    };
}

#endif
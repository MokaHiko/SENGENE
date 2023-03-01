#ifndef PHYSICS_H
#define PHYSICS_H

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Body.h"
#include "Collider.h"

namespace flg
{
    struct PhysicsWorldProperties
    {
        glm::vec3 Gravity = {0.0f, -9.81f, 0.0f};
    };

    // World that holds a reference to all physics bodies
    class PhysicsWorld
    {
    public:
        PhysicsWorld(const PhysicsWorld &other) = delete;
        void operator=(const PhysicsWorld &other) = delete;
        ~PhysicsWorld();

        static void Step(float dt);
        static void AddBody(Body *body);
        static void RemoveBody(Body *body);
        static void Clear();

    public:
        struct Raycasthit
        {
            glm::vec3 CollisionPoint = {};
            Body *body;

            bool DidHit() { return body != nullptr; };
        };

        static Raycasthit Raycast(const Ray *ray, float distance = 1000.0f);

    private:
        PhysicsWorld() {}
        static std::vector<Body *> m_Bodies;
        static PhysicsWorldProperties m_Properties;
    };
}

#endif
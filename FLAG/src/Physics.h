#ifndef PHYSICS_H
#define PHYSICS_H

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <functional>

#include "Body.h"
#include "Collider.h"

namespace flg
{
    struct PhysicsWorldProperties
    {
        glm::vec3 Gravity = glm::vec3{0.0f, -9.81f, 0.0f} * 5.0f;
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

        using CollisionCallbackFn = std::function<void(flg::CollisionPoints &col, uint32_t entityA, uint32_t entityB)>;
        static void SetOnCollisionEnterCallBack(CollisionCallbackFn onEnterFn);
        static void SetOnCollisionExitCallBack(CollisionCallbackFn onExit);

    public:
        struct Raycasthit
        {
            glm::vec3 CollisionPoint = {};
            Body *body;

            bool DidHit() { return body != nullptr; };
        };

        static Raycasthit Raycast(const Ray *ray, float distance = 1000.0f);
        static std::function<void(CollisionPoints &col, uint32_t entityA, uint32_t entityB)> m_CollisionEnterCallback;
        static std::function<void(CollisionPoints &col, uint32_t entityA, uint32_t entityB)> m_CollisionExitCallback;

    private:
        static void ResolveCollision(float dt);

    private:
        PhysicsWorld() {}
        static std::vector<Body *> m_Bodies;
        static PhysicsWorldProperties m_Properties;
    };
}

#endif
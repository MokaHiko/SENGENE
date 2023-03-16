#ifndef UNIT_H
#define UNIT_H

#pragma once

#include "SGE/SGE.h"
#include "SGE/Renderer/ResourceManager.h"
#include "CameraController.h"

#include <random>

class Unit : public SGE::ScriptableEntity
{
public:
    virtual void Select()
    {
        m_IsSelected = true;
    }

    virtual void Deselect()
    {
        m_IsSelected = false;
    }

    virtual void OnUpdate(SGE::TimeStep timestep) override
    {
        if (m_IsDead)
            return;
        if (m_Health <= 0)
        {
            Die();
            return;
        }

        // Action Delays
        m_ActionTime += timestep.GetSeconds();
        if (m_ActionTime >= m_ActionDelay)
        {
            if (!m_UnitActionQueue.empty())
            {
                auto &action = m_UnitActionQueue.front();
                action();
                m_UnitActionQueue.pop();
            }
            m_ActionTime = 0.0f;
        }

        ProcessInput();
        ProcessMovement(timestep);
    };

    virtual void OnStart() override
    {
        m_Health = 100.0f;
        m_Damage = 50.0f;
    }

    virtual bool OnCollisionEnter(flg::CollisionPoints &colPoints, SGE::Entity colEntity) override
    {
        Unit *otherUnit = colEntity.GetNativeScriptComponent<Unit>();
        if (otherUnit != nullptr)
        {
            if (IsFriendly(otherUnit))
            {
                Breed(otherUnit);
            }
            else
            {
                Battle(otherUnit);
            }
        }
        return false;
    };

    ~Unit() {}

private:
    void ProcessInput()
    {
        if (!m_IsSelected)
            return;

        if (SGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
        {
            SGE::Entity camera = SGE::Renderer::GetSceneData().MainCamera;
            CameraController *cameraController = camera.GetNativeScriptComponent<CameraController>();
            if (!cameraController)
                return;

            glm::vec3 rayDir = cameraController->MouseToWorldCoordinates();
            auto ray = flg::Ray(camera.GetComponent<SGE::TransformComponent>().Position, rayDir);

            auto hit = flg::PhysicsWorld::Raycast(&ray, 1000);
            if (hit.DidHit())
            {
                glm::vec3 colPoint = hit.CollisionPoint;
                SGE::Entity hitEntity = {hit.body->GetEntityOwnerID(), GameObject().GetSceneHandle()};
                Goto({colPoint.x, GameObject().GetComponent<SGE::TransformComponent>().Position.y, colPoint.z});
            }
        }
    }

    void ProcessMovement(SGE::TimeStep timestep)
    {
        if (m_IsMoving)
        {
            glm::vec3 direction = m_Destination - GameObject().GetComponent<SGE::TransformComponent>().Position;
            if (glm::length(direction) <= 0.05f)
            {
                GameObject().GetComponent<SGE::RigidBodyComponent>().Body.Velocity = {0, 0, 0};
                m_IsMoving = false;

                // TODO: Remove Debug Message
                auto tag = GameObject().GetComponent<SGE::TagComponent>();
                std::cout << tag.Tag.c_str() << "arrived at locatio!\n";
            }
            else
            {
                GameObject().GetComponent<SGE::RigidBodyComponent>().Body.Velocity = glm::normalize(direction) * m_Velocity * 10.0f * timestep.GetSeconds();
            }
        }
    }

    void Goto(const glm::vec3 &destination)
    {
        m_Destination = destination;
        m_IsMoving = true;
    }

    // [Unit Action]
    void Battle(Unit *enemy)
    {
        enemy->m_Health -= m_Damage;
    }

    // [Unit Action]
    void Breed(Unit *ally)
    {
        if (!m_UnitActionQueue.size() < maxActionsQueue)
            return;

        static float i = 1;
        m_UnitActionQueue.emplace([&]()
                                  {
            SGE::Entity e = GameObject().GetSceneHandle()->CreateEntity(std::string("Baby_", i), GetComponent<SGE::TransformComponent>().Position + (glm::vec3(10.0, 0.0, 10.0) * i));
            e.AddNativeScriptComponent<Unit>();
            e.AddComponent<SGE::MeshRendererComponent>(SGE::Model::CreateModel("assets/models/red_cube/redcube.obj", false));
            e.AddComponent<SGE::RigidBodyComponent>();
            e.AddComponent<SGE::SphereColliderComponent>().sphereCollider.Radius = 1.0f; 
            i++; });
    }

    // [Interupt Action]
    void Die()
    {
        // Stop All Movement
        GameObject().GetComponent<SGE::RigidBodyComponent>().Body.Velocity = {0, 0, 0};
        m_IsMoving = false;

        auto tag = GameObject().GetComponent<SGE::TagComponent>();
        std::cout << tag.Tag.c_str() << " is dead!\n";
        m_IsDead = true;
    }

    // [Check]
    bool IsFriendly(Unit *)
    {
        return true;
    }

private:
    // General
    bool m_IsSelected = false;
    bool m_CanAct = false;
    bool m_IsDead = false;

    int m_Sex = 0;

    float m_ActionDelay = 2.0f;
    float m_ActionTime = 0.0f;

    // Vitals
    float m_Health = 0.0f;
    float m_Fertility = 100.0f;

    // Combat
    float m_Damage = 0.0f;

    // Movement & Navigation
    glm::vec3 m_Destination = {};
    float m_Velocity = 250.0f;
    bool m_IsMoving = false;

    // Action Queue
    std::queue<std::function<void()>> m_UnitActionQueue;
    int maxActionsQueue = 1;
};

#endif
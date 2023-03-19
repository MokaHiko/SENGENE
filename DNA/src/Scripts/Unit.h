#ifndef UNIT_H
#define UNIT_H

#pragma once

#include "CameraController.h"
#include "SGE/Renderer/ResourceManager.h"
#include "SGE/SGE.h"

#include <random>

#include "Food.h"

class Unit : public SGE::ScriptableEntity
{
public:
    virtual void OnUpdate(SGE::TimeStep timestep) override
    {
        if (m_IsDead)
            return;
        if (m_Health <= 0)
        {
            Die();
            return;
        }

        // Update Action Cooldown
        m_ActionTime += timestep.GetSeconds();
        if (!m_IsDisabled)
        {
            if (m_ActionTime >= m_ActionDelay)
            {
                if (!m_UnitActionQueue.empty())
                {
                    auto &action = m_UnitActionQueue.front();
                    action();
                    m_UnitActionQueue.pop();
                }
                else if (!m_IsSelected)
                {
                    GenerateUnitAction();
                }

                // Calculate Energy Usage
                m_Health -= 0.5f;
                m_ActionTime = 0.0f;
            }

            ProcessInput();
            ProcessGoTo(timestep);
        }
        else
        {
            if (GameObject().GetComponent<SGE::TransformComponent>().Position.y <= 1.4f)
                m_IsDisabled = false;
        }
    };

    virtual void OnStart() override
    {
        Reset();
    }

    virtual bool OnCollisionEnter(flg::CollisionPoints &colPoints,
                                  SGE::Entity colEntity) override
    {
        // Unit Actions
        Unit *otherUnit = colEntity.GetNativeScriptComponent<Unit>();
        if (otherUnit != nullptr)
        {
            if (IsFriendly(otherUnit))
                return Breed(otherUnit);
            else
                return Battle(otherUnit);
            return false;
        }

        // Environment Actions
        Food *food = colEntity.GetNativeScriptComponent<Food>();
        if (food != nullptr)
        {
            return Eat(food);
        }
        return false;
    };

    ~Unit() {}

public:
    virtual void Select() { m_IsSelected = true; }
    virtual void Deselect() { m_IsSelected = false; }

private:
    void Reset()
    {
        // Reset Movement
        m_InTransit = false;

        // Reset Combat Stats
        m_Health = 100.0f;
        m_Damage = 1.0f;
        m_IsDisabled = false;

        // Reset General
        m_Sex = rand() % 2;
        m_IsDead = false;
        m_IsSelected = false;

        // Reset RigidBody to Kinematic for regular movement
        auto &rb = GameObject().GetComponent<SGE::RigidBodyComponent>();
        rb.Body.SetPosition(glm::vec3{(rand() - RAND_MAX / 2) % 10, 1.4f, (rand() - RAND_MAX / 2) % 10});
        rb.Body.Type = flg::BodyType::Dynamic;
    }
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
            auto ray = flg::Ray(
                camera.GetComponent<SGE::TransformComponent>().Position, rayDir);

            auto hit = flg::PhysicsWorld::Raycast(&ray, 1000);
            if (hit.DidHit())
            {
                glm::vec3 colPoint = hit.CollisionPoint;
                SGE::Entity hitEntity = {hit.body->GetEntityOwnerID(),
                                         GameObject().GetSceneHandle()};
                Goto({colPoint.x,
                      GameObject().GetComponent<SGE::TransformComponent>().Position.y,
                      colPoint.z});
            }
        }
    }

    void ProcessGoTo(SGE::TimeStep timestep)
    {
        if (m_InTransit)
        {
            glm::vec3 direction = m_Destination - GameObject().GetComponent<SGE::TransformComponent>().Position;
            if (glm::length(direction) <= 2.0f)
            {
                auto &rb = GameObject().GetComponent<SGE::RigidBodyComponent>();
                rb.Body.Velocity = glm::vec3(0.0f);
                rb.Body.Force = glm::vec3(0.0f);
                m_InTransit = false;
            }
            else
            {
                auto &rb = GameObject().GetComponent<SGE::RigidBodyComponent>();
                glm::vec3 velocity = direction * m_MovementSpeed * 1000.0f * timestep.GetSeconds();

                if (glm::abs(velocity.x) > m_MovementSpeed)
                    rb.Body.Velocity.x = m_MovementSpeed * (glm::sign(velocity.x));
                if (glm::abs(velocity.z) > m_MovementSpeed)
                    rb.Body.Velocity.z = m_MovementSpeed * (glm::sign(velocity.z));
                rb.Body.Velocity.y = 0;
            }
        }
    }

    void Goto(const glm::vec3 &destination)
    {
        m_Destination = destination;
        m_InTransit = true;
    }

    void GenerateUnitAction()
    {
        if (!(static_cast<uint32_t>(m_UnitActionQueue.size()) < maxActionsQueue))
            return;

        // Levy Pattern
        m_LocalSearchMoves++;
        if (m_LocalSearchMoves > 5)
        {
            m_LocalSearchMoves = 0;
            m_UnitActionQueue.emplace([&]()
                                      { 
                                        auto& position = GameObject().GetComponent<SGE::TransformComponent>().Position;
                                        glm::vec3 nextDestination = position + glm::vec3{(rand() - RAND_MAX/2) % m_ExtendedSearchRange , position.y, (rand() - RAND_MAX/2) % m_ExtendedSearchRange};
                                        Goto(nextDestination); });
        }
        else
        {
            m_UnitActionQueue.emplace([&]()
                                      { 
                                        auto& position = GameObject().GetComponent<SGE::TransformComponent>().Position;
                                        glm::vec3 nextDestination = position + glm::vec3{(rand() - RAND_MAX/2) % m_SearchRange, position.y, (rand() - RAND_MAX/2) % m_SearchRange};
                                        Goto(nextDestination); });
        }
    }

    // [Unit Action]
    bool Battle(Unit *enemy)
    {
        auto &rb = GameObject().GetComponent<SGE::RigidBodyComponent>();
        auto &enemyRb = enemy->GetComponent<SGE::RigidBodyComponent>();

        // Launch enemy
        glm::vec3 forceDir = enemyRb.Body.GetPosition() - rb.Body.GetPosition();
        forceDir *= 250.0f;
        forceDir.y = 1000.0f * 1.5f;
        enemyRb.AddImpulse(forceDir);

        // Disable Current
        enemy->m_InTransit = false;
        enemy->m_IsDisabled = true;

        // Deal Damage
        enemy->m_Health -= m_Damage;
        return false;
    }

    // [Unit Action]
    bool Breed(Unit *ally)
    {
        if (!m_UnitActionQueue.size() < maxActionsQueue)
            return false;

        // Population Control
        if (m_BreadCount > 250)
            return false;

        m_UnitActionQueue.emplace([&]()
                                  {
        SGE::Entity e = GameObject().GetSceneHandle()->CreateEntity(GameObject(), std::string("Baby_", m_BreadCount));
        e.GetComponent<SGE::TransformComponent>().Position +(glm::vec3(10.0, 1.4f, 10.0) * m_BreadCount); 
        e.AddNativeScriptComponent<Unit>(); });

        m_BreadCount++;

        return true;
    }

    //[Environment Action]
    bool Eat(Food *food)
    {
        FoodProperties &props = food->Eat();
        m_Health += props.HealthRegen;

        return true;
    }

    // [Interupt Action]
    void Die()
    {
        // Stop All Movement
        m_InTransit = false;
        m_IsDead = true;

        GameObject().GetComponent<SGE::RigidBodyComponent>().Body.BodyTransform.Position.y = (100.0f);
        GameObject().GetComponent<SGE::RigidBodyComponent>().Body.Type = flg::Static;
    }

    // [Check]
    bool IsFriendly(Unit *otherUnit)
    {
        return !(otherUnit->m_Sex & m_Sex);
    }

private:
    // General
    bool m_IsSelected = false;
    bool m_IsDead = false;

    int m_Sex = 0;

    float m_ActionDelay = 0.25f;
    float m_ActionTime = 0.0f;

    // Vitals
    float m_Health = 0.0f;
    float m_Fertility = 100.0f;

    // Combat
    bool m_IsDisabled = false;
    float m_Damage = 25.0f;

    // Movement & Navigation
    glm::vec3 m_Destination = {};
    float m_MovementSpeed = 5.0f;
    bool m_InTransit = false;

    int m_SearchRange = 5;
    int m_ExtendedSearchRange = 25;
    int m_LocalSearchMoves = 0;

    // Action Queue
    std::queue<std::function<void()>> m_UnitActionQueue;
    int maxActionsQueue = 1;

    static float m_BreadCount;
};

#endif
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
    ~Unit() {}
    virtual void OnUpdate(SGE::TimeStep timestep)
    {
        if (SGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
        {
            if (SGE::Renderer::GetSceneData().MainCamera.HasComponent<SGE::NativeScriptComponent>())
            {
                SGE::Entity camera = SGE::Renderer::GetSceneData().MainCamera;
                CameraController *cameraController = (CameraController *)((void *)(camera.GetComponent<SGE::NativeScriptComponent>().ScriptInstance));
                if (!cameraController)
                    return;

                glm::vec3 rayDir = cameraController->MouseToWorldCoordinates();
                auto ray = flg::Ray(camera.GetComponent<SGE::TransformComponent>().Position, rayDir);

                auto hit = flg::PhysicsWorld::Raycast(&ray, 10000);
                if (hit.DidHit())
                {
                    glm::vec3 colPoint = hit.CollisionPoint;
                    SGE::Entity hitEntity = {hit.body->GetEntityOwnerID(), GameObject().GetSceneHandle()};
                    m_Destination = colPoint;
                    m_IsMoving = true;
                }
            }
        }

        if (m_IsMoving)
        {
            glm::vec3 direction = m_Destination - GameObject().GetComponent<SGE::TransformComponent>().Position;
            if (direction.length() < 0.001f)
            {
                GameObject().GetComponent<SGE::RigidBodyComponent>().Body.Velocity = {0, 0, 0};
                m_IsMoving = false;
            }
            else
            {
                GameObject().GetComponent<SGE::RigidBodyComponent>().Body.Velocity = glm::normalize(direction) * m_Velocity * 10.0f * timestep.GetSeconds();
            }
        }
    };

    virtual void OnStart() override
    {
        // Add Model Component
        GameObject().AddComponent<SGE::SkinnedMeshRendererComponent>(SGE::ResourceManager::GetAnimatedModel("assets/models/mutant/mutant.fbx"));
        GameObject().GetComponent<SGE::TransformComponent>().Scale = glm::vec3(0.05, 0.05, 0.05);
    }

private:
    float m_Time = 0.0f;
    glm::vec3 m_Destination = {};
    float m_Velocity = 250.0f;
    bool m_IsMoving = false;
};

#endif
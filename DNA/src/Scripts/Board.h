#ifndef BOARD_H
#define BOARD_H

#pragma once
#include "SGE/SGE.h"

#include "Unit.h"
#include "Scene/Components.h"

class Board : public SGE::ScriptableEntity
{
public:
    Board() {}
    ~Board() {}

    virtual void OnStart() override
    {
        // Game Parameters

        // Spawn Map
        SGE::Entity plane = GameObject().GetSceneHandle()->CreateEntity("Plane");
        plane.AddComponent<SGE::MeshRendererComponent>(SGE::ResourceManager::GetModel("assets/models/cube/cube.obj"));
        plane.GetComponent<SGE::TransformComponent>().Scale = {100.0f, 1.0, 100.0f};
        plane.AddComponent<SGE::RigidBodyComponent>().Body.BodyTransform.Position = plane.GetComponent<SGE::TransformComponent>().Position;
        plane.AddComponent<SGE::PlaneColliderComponent>();

        // Spawn Grass
        glm::vec2 grassDim = {10.0f, 10.0f};
        float grassSpacing = 2.0f;

        for (uint32_t i = 0; i < grassDim.x; i++)
        {
            for (uint32_t j = 0; j < grassDim.y; j++)
            {
                std::string name = "Grass_";
                name.push_back(char(i));
                name.push_back(char(j));

                SGE::Entity grass = GameObject().GetSceneHandle()->CreateEntity(name, glm::vec3(i * grassSpacing, 3, j * grassSpacing));
                grass.GetComponent<SGE::TransformComponent>().Scale *= 2;
                grass.AddComponent<SGE::MeshRendererComponent>(SGE::ResourceManager::CreateModel("assets/models/Billboard_grass/BillBoardGrass.obj", true));
            }
        }

        // Spawn Units
        glm::vec2 boardDim = {5, 5};
        float pieceSize = 0.5f;

        for (uint32_t i = 0; i < boardDim.x; i++)
        {
            for (uint32_t j = 0; j < boardDim.y; j++)
            {
                std::string name = "Unit_";
                name.push_back(char(48 + i));
                name.push_back(char(48 + j));

                // Bind cell script
                SGE::Entity e = GameObject().GetSceneHandle()->CreateEntity(name, glm::vec3(i * 10, 0, j * 10));
                e.AddComponent<SGE::NativeScriptComponent>().Bind<Unit>();
                e.AddComponent<SGE::SphereColliderComponent>();
                e.AddComponent<SGE::RigidBodyComponent>();

                m_Cells[i][j] = e;
            }
        }
    }

    virtual void OnUpdate(SGE::TimeStep timeStep)
    {
    }

private:
    std::array<std::array<SGE::Entity, 8>, 8> m_Cells = {};
    bool m_Initialized = false;
};

#endif
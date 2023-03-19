#ifndef BOARD_H
#define BOARD_H

#pragma once
#include "OpenSimplexNoise.h"
#include "SGE/SGE.h"

#include "GrassRenderer/GrassRenderer.h"
#include "Scene/Components.h"
#include "Unit.h"
#include <random>

class Board : public SGE::ScriptableEntity
{
public:
  ~Board() {}
  virtual void OnCreate() override
  {
    // Set Random Seed
    srand(static_cast<uint32_t>(glfwGetTime()));

    // Spawn Map
    SGE::Entity plane = GameObject().GetSceneHandle()->CreateEntity("Plane", glm::vec3(0.0f, -0.5f, 0.0f));
    auto &meshRenderer = plane.AddComponent<SGE::MeshRendererComponent>(SGE::ResourceManager::GetModel("assets/models/cube/cube.obj"));
    auto &checkerboardMaterial = SGE::Material::CreateMaterial("CheckerBoard");
    checkerboardMaterial->DiffuseTexture = SGE::Texture2D::CreateTexture2D("assets/textures/tile.png");
    checkerboardMaterial->DiffuseColor = glm::vec3(1.0f);
    checkerboardMaterial->SpecularColor = glm::vec3(1.0f);
    meshRenderer.Model->SetMaterial(checkerboardMaterial);

    plane.GetComponent<SGE::TransformComponent>().Scale = {100.0f, 0.0, 100.0f};
    plane.AddComponent<SGE::RigidBodyComponent>().Body.BodyTransform.Position = plane.GetComponent<SGE::TransformComponent>().Position;
    plane.AddComponent<SGE::PlaneColliderComponent>();

    // Spawn Grass
    glm::vec2 grassDim = {300.0f, 300.0f};
    float grassScale = 7.5f;
    float grassSpacing = 0.25f;

    OpenSimplexNoise::Noise noise;

    for (uint32_t i = 0; i < grassDim.x; i++)
    {
      for (uint32_t j = 0; j < grassDim.y; j++)
      {
        glm::vec3 scale = glm::vec3(grassScale);
        scale.y = static_cast<float>(noise.eval(i, j)) * 1.0f;
        glm::vec3 position = glm::vec3(i * grassSpacing, 0, j * grassSpacing);
        position.x += (float)(rand() % (int)grassScale) * grassSpacing * scale.y * 0.5f;
        position.z += (float)(rand() % (int)grassScale) * grassSpacing * scale.y * 0.5f;
        glm::vec3 rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
        SGE::GrassRenderer::AddInstance(position, rotation, scale);
      }
    }

    SGE::Model::CreateModel("./assets/models/apple/Apple.fbx", true);

    // Spawn Units
    glm::vec2 boardDim = {2.0f, 2.0f};
    float unitSpacing = 5.0f;

    for (uint32_t i = 0; i < boardDim.x; i++)
    {
      for (uint32_t j = 0; j < boardDim.y; j++)
      {
        std::stringstream s;
        s << "Unit_" << i << '_' << j;

        SGE::Entity e = GameObject().GetSceneHandle()->CreateEntity(s.str(), glm::vec3(i * unitSpacing, 0.4f, j * unitSpacing));
        e.AddNativeScriptComponent<Unit>();
        e.AddComponent<SGE::MeshRendererComponent>(SGE::Model::CreateModel("assets/models/slime/slime.fbx", true));
        e.AddComponent<SGE::RigidBodyComponent>().Body.Type = flg::BodyType::Dynamic;
        e.AddComponent<SGE::SphereColliderComponent>().sphereCollider.Radius = 1.0f;
        e.GetComponent<SGE::TransformComponent>().Scale *= 0.02f;
      }
    }
  }

  virtual void OnStart() override
  {
    static uint32_t MAX_SELECTED_UNITS = 100;
    m_Selected.resize(MAX_SELECTED_UNITS);

    // Spawn Food
    for (int32_t i = 0; i < 10.0f; i++)
    {
      SGE::Entity e = GameObject().GetSceneHandle()->CreateEntity("Food");
      e.AddNativeScriptComponent<Food>();
      e.AddComponent<SGE::MeshRendererComponent>(SGE::ResourceManager::GetModel("./assets/models/apple/Apple.fbx"));
      e.AddComponent<SGE::RigidBodyComponent>();
      e.AddComponent<SGE::SphereColliderComponent>();
      e.GetComponent<SGE::TransformComponent>().Scale *= 0.1f;
    }
  }

  virtual void OnUpdate(SGE::TimeStep timeStep) { ProcessInput(); }

  void ProcessInput()
  {
    if (SGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
    {
      SGE::Entity camera = SGE::Renderer::GetSceneData().MainCamera;
      CameraController *cameraController =
          camera.GetNativeScriptComponent<CameraController>();
      if (!cameraController)
        return;

      glm::vec3 rayDir = cameraController->MouseToWorldCoordinates();
      auto ray = flg::Ray(
          camera.GetComponent<SGE::TransformComponent>().Position, rayDir);

      auto hit = flg::PhysicsWorld::Raycast(&ray, 10000);
      if (hit.DidHit())
      {
        DeselectAll();

        glm::vec3 colPoint = hit.CollisionPoint;
        SGE::Entity hitEntity = {hit.body->GetEntityOwnerID(),
                                 GameObject().GetSceneHandle()};

        printf("Hit: %s\n",
               hitEntity.GetComponent<SGE::TagComponent>().Tag.c_str());

        if (!hitEntity.HasComponent<SGE::NativeScriptComponent>())
          return;

        Unit *unit = hitEntity.GetNativeScriptComponent<Unit>();
        if (unit != nullptr)
        {
          m_SelectCount = 1;
          m_Selected[0] = unit;
        }
        else
        {
          m_SelectCount = 0;
        }

        for (uint32_t i = 0; i < m_SelectCount; i++)
        {
          m_Selected[i]->Select();
        }
      }
    }
  }

  void DeselectAll()
  {
    for (uint32_t i = 0; i < m_SelectCount; i++)
    {
      m_Selected[i]->Deselect();
    }
  }

private:
  bool m_Initialized = false;

  std::vector<Unit *> m_Selected = {};
  uint32_t m_SelectCount = 0;
};

#endif
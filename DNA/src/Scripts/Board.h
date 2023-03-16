#ifndef BOARD_H
#define BOARD_H

#pragma once
#include "OpenSimplexNoise.h"
#include "SGE/SGE.h"

#include "GrassRenderer/GrassRenderer.h"
#include "Scene/Components.h"
#include "Unit.h"
#include "random"

class Board : public SGE::ScriptableEntity {
public:
  Board() {}
  ~Board() {}

  virtual void OnStart() override {
    static uint32_t MAX_SELECTED_UNITS = 100;
    m_Selected.resize(MAX_SELECTED_UNITS);
  }

  virtual void OnUpdate(SGE::TimeStep timeStep) { ProcessInput(); }

  void ProcessInput() {
    if (SGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
      SGE::Entity camera = SGE::Renderer::GetSceneData().MainCamera;
      CameraController *cameraController =
          camera.GetNativeScriptComponent<CameraController>();
      if (!cameraController)
        return;

      glm::vec3 rayDir = cameraController->MouseToWorldCoordinates();
      auto ray = flg::Ray(
          camera.GetComponent<SGE::TransformComponent>().Position, rayDir);

      auto hit = flg::PhysicsWorld::Raycast(&ray, 10000);
      if (hit.DidHit()) {
        DeselectAll();

        glm::vec3 colPoint = hit.CollisionPoint;
        SGE::Entity hitEntity = {hit.body->GetEntityOwnerID(),
                                 GameObject().GetSceneHandle()};

        printf("Hit: %s\n",
               hitEntity.GetComponent<SGE::TagComponent>().Tag.c_str());

        if (!hitEntity.HasComponent<SGE::NativeScriptComponent>())
          return;

        Unit *unit = hitEntity.GetNativeScriptComponent<Unit>();
        if (unit != nullptr) {
          m_SelectCount = 1;
          m_Selected[0] = unit;
        } else {
          m_SelectCount = 0;
        }

        for (uint32_t i = 0; i < m_SelectCount; i++) {
          m_Selected[i]->Select();
        }
      }
    }
  }

  void DeselectAll() {
    for (uint32_t i = 0; i < m_SelectCount; i++) {
      m_Selected[i]->Deselect();
    }
  }

private:
  bool m_Initialized = false;

  std::vector<Unit *> m_Selected = {};
  uint32_t m_SelectCount = 0;
};

#endif
#ifndef COMPONENTS_H
#define COMPONENTS_H

#pragma once
#include <glm/glm.hpp>

#include "Core/Core.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Scene/ScriptableEntity.h"
#include "Events/Event.h"

namespace SGE {
   struct TransformComponent
   {
        glm::vec3 Position = {0.0f, 0.0f, 0.0f};
        glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 Scale    = {1.0f, 1.0f, 1.0f};
   };

   struct MeshRendererComponent
   {
      Ref<Model> Model;
      MeshRendererComponent(Ref<SGE::Model> model)
      :Model(model){}
   };

   struct PointLightComponent
   {
      float Constant = 1.0f;
      float Linear = 0.09f;
      float Quadratic = 0.032f;

      glm::vec3 Ambient{0.05f};
      glm::vec3 Diffuse{0.8f};
      glm::vec3 Specular{1.0f};
   };

   struct DirectionalLightComponent
   {
      glm::vec3 Ambient{0.8f};
      glm::vec3 Diffuse{0.8f};
      glm::vec3 Specular{1.0f};
   };

   struct TagComponent
   {
      std::string Tag;
      TagComponent(const std::string& name)
         :Tag(name) {}
   };

   struct Camera3DComponent
   {
      bool IsActive;
      Camera3D camera{};

      Camera3DComponent(bool isActive = true)
         :IsActive(isActive) {}
   };

   struct NativeScriptComponent
   {
      ScriptableEntity* ScriptInstance = nullptr;

      ScriptableEntity*(*InstantiateScript)() = nullptr;
      void(*DestroyScript)(NativeScriptComponent*) = nullptr;

      template<typename T>
      void Bind()
      {
         InstantiateScript = [](){return static_cast<ScriptableEntity*>(new T());};
         DestroyScript = [](NativeScriptComponent* nsc){delete nsc->ScriptInstance; nsc->ScriptInstance = nullptr;};
      }
   };

   template<typename T>
   struct EventWatcherComponent
   {
      std::function<void(T&)> EntityCallBack;

      EventWatcherComponent() = default;

      void Watch(std::function<void(T&)> callback)
      {
          bool isEvent = std::is_base_of<Event, T>::value;
          assert(isEvent);
          EntityCallBack = callback;
      }
   };
}

#endif
#ifndef COMPONENTS_H
#define COMPONENTS_H

#pragma once
#include <glm/glm.hpp>

#include "Core/Core.h"
#include "Renderer/Model.h"
#include "Renderer/SkinnedMeshRenderer/AnimatedModel.h"
#include "Renderer/Camera.h"
#include "Scene/ScriptableEntity.h"
#include "Events/Event.h"
#include "Physics.h"

namespace SGE
{
   struct TransformComponent
   {
      glm::vec3 Position = {0.0f, 0.0f, 0.0f};
      glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
      glm::vec3 Scale = {1.0f, 1.0f, 1.0f};
   };

   struct MeshRendererComponent
   {
      Ref<Model> Model;
      bool FlipUVS;
      MeshRendererComponent(Ref<SGE::Model> model, bool flipUVS = false)
          : Model(model), FlipUVS{flipUVS} {}
   };

   struct SkinnedMeshRendererComponent
   {
      Ref<AnimatedModel> AnimatedModel;
      bool FlipUVS;
      SkinnedMeshRendererComponent(Ref<SGE::AnimatedModel> model, bool flipUVS = false)
          : AnimatedModel(model), FlipUVS{flipUVS} {}
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
      glm::vec3 Ambient{0.3f};
      glm::vec3 Diffuse{0.8f};
      glm::vec3 Specular{0.2f};
   };

   struct TagComponent
   {
      static const uint32_t MAX_TAG_SIZE = 128; // in bytes

      std::string Tag;
      TagComponent(const std::string &name)
          : Tag(name) {}
   };

   struct Camera3DComponent
   {
      bool IsActive;
      Camera3D camera{};

      Camera3DComponent(bool isActive = true)
          : IsActive(isActive) {}
   };

   struct NativeScriptComponent
   {
      ScriptableEntity *ScriptInstance = nullptr;

      ScriptableEntity *(*InstantiateScript)() = nullptr;
      void (*DestroyScript)(NativeScriptComponent *) = nullptr;

      template <typename T>
      void Bind()
      {
         InstantiateScript = []()
         { return static_cast<ScriptableEntity *>(new T()); };
         DestroyScript = [](NativeScriptComponent *nsc)
         {delete nsc->ScriptInstance; nsc->ScriptInstance = nullptr; };
      }
   };

   template <typename T>
   struct EventWatcherComponent
   {
      std::function<void(T &)> EntityCallBack;

      EventWatcherComponent() = default;

      void Watch(std::function<void(T &)> callback)
      {
         bool isEvent = std::is_base_of<Event, T>::value;
         assert(isEvent);
         EntityCallBack = callback;
      }
   };

   struct RigidBodyComponent
   {
      flg::Body Body;
      bool UseGravity = true;
      bool Registered = false;

      RigidBodyComponent() = default;
      RigidBodyComponent(const RigidBodyComponent &other) = default;
   };

   struct SphereColliderComponent
   {
      flg::SphereCollider sphereCollider;

      SphereColliderComponent() = default;
      SphereColliderComponent(const SphereColliderComponent &other) = default;
   };

   struct PlaneColliderComponent
   {
      flg::PlaneCollider planeCollider;

      PlaneColliderComponent() = default;
      PlaneColliderComponent(const PlaneColliderComponent &other) = default;
   };
}

#endif
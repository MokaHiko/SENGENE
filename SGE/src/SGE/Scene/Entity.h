#ifndef ENTITY_H
#define ENTITY_H

#pragma once
#include <entt/entt.hpp>

namespace SGE
{
  class Scene;
  class Entity
  {
  public:
    Entity() = default;
    Entity(entt::entity entityHandle, Scene *scene);
    Entity(uint32_t entityID, Scene *scene);

    template <typename T, typename... Args>
    T &AddComponent(Args &&...args)
    {
      if (HasComponent<T>())
        return GetComponent<T>();

      return m_Scene->Registry().emplace<T>(m_EntityHandle,
                                            std::forward<Args>(args)...);
    }

    template <typename T>
    T &GetComponent()
    {
      return m_Scene->Registry().get<T>(m_EntityHandle);
    }

    template <typename T>
    T *AddNativeScriptComponent()
    {
      if (HasComponent<NativeScriptComponent>())
        return GetNativeScriptComponent<T>();

      m_Scene->Registry()
          .emplace<NativeScriptComponent>(m_EntityHandle)
          .Bind<T>();
      return GetNativeScriptComponent<T>();
    }

    template <typename T>
    T *GetNativeScriptComponent()
    {
      if (!HasComponent<NativeScriptComponent>())
        return nullptr;

      // return (T *)(void *)(GetComponent<NativeScriptComponent>().ScriptInstance);
      return dynamic_cast<T *>(GetComponent<NativeScriptComponent>().ScriptInstance);
    }

    template <typename T>
    bool HasComponent()
    {
      return m_Scene->Registry().any_of<T>(m_EntityHandle);
    }

    bool operator==(const Entity &other) const
    {
      return other.m_EntityHandle == m_EntityHandle;
    }

    operator bool() const { return m_EntityHandle != entt::null; }

    const uint32_t Id() const { return static_cast<uint32_t>(m_EntityHandle); };
    Scene *GetSceneHandle() const { return m_Scene; };

  private:
    entt::entity m_EntityHandle = entt::null;
    Scene *m_Scene = nullptr;

    friend class Scene;
  };
} // namespace SGE

#endif
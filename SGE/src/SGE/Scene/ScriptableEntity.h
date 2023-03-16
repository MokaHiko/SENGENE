#ifndef SCRIPTABLEENTITY_H
#define SCRIPTABLEENTITY_H

#pragma once
#include "Core/TimeStep.h"
#include "Entity.h"
#include "Physics.h"

namespace SGE {
class ScriptableEntity {
public:
  virtual ~ScriptableEntity() { std::cout << "nice" << std::endl; };

  template <typename T> T &GetComponent() { return m_Entity.GetComponent<T>(); }

  template <typename T, typename... Args> T &AddComponent(Args &&...args) {
    return m_Entity.AddComponent<T>(std::forward<Args>(args)...);
  }

  template <typename T> T *GetNativeScriptComponent() {
    return m_Entity.GetNativeScriptComponent<T>();
  }

  virtual void OnCreate(){};
  virtual void OnStart(){};
  virtual void OnDestroy(){};
  virtual void OnUpdate(TimeStep timestep){};

  virtual bool OnCollisionEnter(flg::CollisionPoints &colPoints,
                                Entity colEntity) {
    return false;
  };

  Entity GameObject() { return m_Entity; };

private:
  Entity m_Entity{};
  friend class Scene;
};
} // namespace SGE

#endif
#ifndef ENTITY_H
#define ENTITY_H

#pragma once

#include <entt/entt.hpp>

namespace SGE {
    class Scene;
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity entityHandle, Scene* scene);
        Entity(uint32_t entityID, Scene* scene);

        template <typename T, typename ... Args>
        T& AddComponent(Args&& ... args)
        {
            if (HasComponent<T>())
                GetComponent<T>();

            return m_Scene->Registry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template <typename T>
        T& GetComponent()
        {
            return m_Scene->Registry().get<T>(m_EntityHandle);
        }

        template <typename T>
        bool HasComponent()
        {
            return m_Scene->Registry().any_of<T>(m_EntityHandle);
        }

        bool operator==(const Entity& other) const
        {
            return other.m_EntityHandle == m_EntityHandle;
        }

        operator bool() const
        {
            return m_EntityHandle != entt::null;
        }

    private:
        entt::entity m_EntityHandle = entt::null;
        Scene* m_Scene = nullptr; // TODO: some sort of weak ptr
    };
}

#endif
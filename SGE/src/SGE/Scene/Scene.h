#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "Entity.h"
#include "Components.h"
#include "Renderer/Shader.h"

#include <Physics.h>
#include <glm/glm.hpp>

namespace SGE
{
    struct SceneData
    {
        SceneData()
            : SceneWidth(1280), SceneHeight(720), MainCamera(), DirectionalLight(), FocusedBoneIndex(0), ProjectionMatrix(1.0f), ViewPortBounds() {}

        SceneData(Entity camera, uint32_t width = 1280, uint32_t height = 720)
            : MainCamera(camera), DirectionalLight(), FocusedBoneIndex(0), SceneWidth(width), SceneHeight(height), ProjectionMatrix(1.0f), ViewPortBounds() {}

        // Create Sort of Weak Ptr to replace raw *
        glm::mat4 ProjectionMatrix{};
        glm::vec2 ViewPortBounds[2];

        Entity MainCamera;
        Entity DirectionalLight;
        std::vector<Entity> PointLights;

        uint32_t SceneWidth;
        uint32_t SceneHeight;

        // Gizmos
        uint32_t FocusedBoneIndex;
    };

    enum class SCENE_STATE
    {
        PAUSE,
        PLAY,
    };

    class Scene
    {
    public:
        Scene(const std::string &sceneName = "Scene");
        ~Scene();

        void Update(TimeStep timestep);

        void OnScenePlay();
        void OnSceneStop();

        Entity CreateEntity(const std::string &name = "UNNAMED_ENTITY", const glm::vec3 &position = glm::vec3(0.0f))
        {
            Entity entity = {m_Registry.create(), this};
            entity.AddComponent<TagComponent>(name);
            entity.AddComponent<TransformComponent>(position);
            return entity;
        }

        void RemoveEntity(Entity entity)
        {
            m_EntitiesToDestroy.push_back(entity);

            // auto &transform = entity.AddComponent<TransformComponent>();
            // transform = otherEntity.GetComponent<TransformComponent>();

            // if (otherEntity.HasComponent<TagComponent>())
            //     entity.AddComponent<TagComponent>(otherEntity.GetComponent<TagComponent>().Tag);

            // if (otherEntity.HasComponent<MeshRendererComponent>())
            //     entity.AddComponent<MeshRendererComponent>(otherEntity.GetComponent<MeshRendererComponent>().Model);

            // if (otherEntity.HasComponent<SkinnedMeshRendererComponent>())
            //     entity.AddComponent<SkinnedMeshRendererComponent>(otherEntity.GetComponent<SkinnedMeshRendererComponent>().AnimatedModel);
        }

        Entity CreateEntity(Entity otherEntity, const std::string &name = "UNNAMED_ENTITY")
        {
            Entity entity = {m_Registry.create(), this};

            auto &transform = entity.AddComponent<TransformComponent>();
            transform = otherEntity.GetComponent<TransformComponent>();

            if (otherEntity.HasComponent<TagComponent>())
                entity.AddComponent<TagComponent>(otherEntity.GetComponent<TagComponent>().Tag);

            if (otherEntity.HasComponent<MeshRendererComponent>())
                entity.AddComponent<MeshRendererComponent>(otherEntity.GetComponent<MeshRendererComponent>().Model);

            if (otherEntity.HasComponent<SkinnedMeshRendererComponent>())
                entity.AddComponent<SkinnedMeshRendererComponent>(otherEntity.GetComponent<SkinnedMeshRendererComponent>().AnimatedModel);

            if (otherEntity.HasComponent<RigidBodyComponent>())
                entity.AddComponent<SGE::RigidBodyComponent>();

            if (otherEntity.HasComponent<SphereColliderComponent>())
                entity.AddComponent<SphereColliderComponent>().sphereCollider.Radius = 1.0f;

            return entity;
        }

        void RegisterToPhysicsWorld(Entity e);

        void CollisionEnterCallback(flg::CollisionPoints &col, uint32_t entityA, uint32_t entityB);
        void CollisionExitCallback(flg::CollisionPoints &col, uint32_t entityA, uint32_t entityB);

        entt::registry &Registry() { return m_Registry; }

        SCENE_STATE m_SceneState = SCENE_STATE::PAUSE;

        inline const std::string &GetSceneName() const { return m_Name; }

    private:
        entt::registry m_Registry;
        std::string m_Name;

        std::vector<Entity> m_EntitiesToDestroy;

        friend class Entity;
        friend class SceneSerializer;
    };
}

#endif
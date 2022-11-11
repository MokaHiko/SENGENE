#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "Entity.h"
#include "Components.h"
#include <entt/entt.hpp>
#include "Renderer/Shader.h"

class b2World;
namespace SGE {
    struct SceneData
    {
        SceneData() 
            :SceneWidth(1280), SceneHeight(720), MainCamera(), DirectionalLight(), FocusedBoneIndex(0) {}

        SceneData(Entity camera, uint32_t width = 1280, uint32_t height = 720) 
            :MainCamera(camera), DirectionalLight(), FocusedBoneIndex(0), SceneWidth(width), SceneHeight(height){}
        
        // Create Sort of Weak Ptr to replace raw *
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
        Scene(const std::string& sceneName = "Scene");
        ~Scene();

        void Update(TimeStep timestep);

        void OnScenePlay();
        void OnSceneStop();

        Entity CreateEntity(const std::string& name = "UNNAMED_ENTITY", const glm::vec3& position = glm::vec3(0.0f))
        {
            Entity entity = {m_Registry.create(), this};
            entity.AddComponent<TagComponent>(name);
            entity.AddComponent<TransformComponent>(position);
            return entity;
        }

        Entity CreateEntity(Entity otherEntity, const std::string& name = "UNNAMED_ENTITY")
        {
            Entity entity = { m_Registry.create(), this };

            auto& transform = entity.AddComponent<TransformComponent>();
            transform = otherEntity.GetComponent<TransformComponent>();

            if (otherEntity.HasComponent<TagComponent>())
                entity.AddComponent<TagComponent>(otherEntity.GetComponent<TagComponent>().Tag);

            if (otherEntity.HasComponent<MeshRendererComponent>())
                entity.AddComponent<MeshRendererComponent>(otherEntity.GetComponent<MeshRendererComponent>().Model);

            if (otherEntity.HasComponent<SkinnedMeshRendererComponent>())
                entity.AddComponent<SkinnedMeshRendererComponent>(otherEntity.GetComponent<SkinnedMeshRendererComponent>().AnimatedModel);

            if (otherEntity.HasComponent<BoxCollider2DComponent>())
                entity.AddComponent<BoxCollider2DComponent>(otherEntity.GetComponent<BoxCollider2DComponent>());

            if (otherEntity.HasComponent<RigidBody2DComponent>())
                entity.AddComponent<RigidBody2DComponent>(otherEntity.GetComponent<RigidBody2DComponent>());

            return entity;
        }

        entt::registry& Registry() {return m_Registry;}
        
        SCENE_STATE m_SceneState = SCENE_STATE::PAUSE;

        inline const std::string& GetSceneName() const {return m_Name;}
    private:
        b2World* m_PhysicsWorld = nullptr;
    private:
        entt::registry m_Registry;
        std::string m_Name;
        
        friend class Entity;
        friend class SceneSerializer;
    };
}

#endif
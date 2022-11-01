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
            :SceneWidth(1280), SceneHeight(720), MainCamera(), DirectionalLight() {}

        SceneData(Entity camera, uint32_t width = 1280, uint32_t height = 720) 
            :MainCamera(camera), DirectionalLight(), SceneWidth(width), SceneHeight(height){}
        
        // Create Sort of Weak Ptr to replace raw *
        Entity MainCamera; 
        Entity DirectionalLight;
        std::vector<Entity> PointLights;

        uint32_t SceneWidth;
        uint32_t SceneHeight;

        Ref<Shader> SceneShader = nullptr;
    };

    enum class SCENE_STATE 
    {
        PAUSE,
        PLAY,
    };

    class Scene
    {
    public:
        Scene();
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
        entt::registry& Registry() {return m_Registry;}
        
        SCENE_STATE m_SceneState = SCENE_STATE::PAUSE;
    private:
        b2World* m_PhysicsWorld = nullptr;
    private:
        entt::registry m_Registry;
        
        friend class Entity;
        friend class SceneSerializer;
    };
}

#endif
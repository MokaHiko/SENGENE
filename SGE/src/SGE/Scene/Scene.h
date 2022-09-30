#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "Entity.h"
#include "Components.h"
#include <entt/entt.hpp>
#include "Renderer/Shader.h"

namespace SGE {
    struct SceneData
    {
        SceneData() 
            :SceneWidth(1280), SceneHeight(720){}

        SceneData(Entity camera, uint32_t width = 1280, uint32_t height = 720) 
            :MainCamera(camera), SceneWidth(width), SceneHeight(height){}
        
        // Create Sort of Weak Ptr to replace raw *
        Entity MainCamera; 
        std::vector<Entity> PointLights;

        uint32_t SceneWidth;
        uint32_t SceneHeight;

        Ref<Shader> SceneShader = nullptr;
    };

    class Scene
    {
    public:
        Scene();
        ~Scene();

        void Update(TimeStep timestep);

        Entity CreateEntity(const std::string& name = "UNNAMED_ENTITY", const glm::vec3& position = glm::vec3(0.0f))
        {
            Entity entity = {m_Registry.create(), this};
            entity.AddComponent<TagComponent>(name);
            entity.AddComponent<TransformComponent>(position);
            return entity;
        }

        entt::registry& Registry() {return m_Registry;}
    private:
        friend class Entity;

        entt::registry m_Registry;
    };
}

#endif
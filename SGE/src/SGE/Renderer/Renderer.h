#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include <glad/glad.h>

#include "Core/Core.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"

#include "Scene/Scene.h"

namespace SGE{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        static void Init();

        static void Configure(SceneData& sceneData);
        static void Begin();
        static void End();

        static void OnWindowResize(uint32_t width, uint32_t height);
    public:
	    static void Draw(Ref<Mesh> mesh, Ref<Material> material,
                        const glm::vec3& position = glm::vec3(1.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));

        struct MaterialGroup
        {
            std::unordered_set<Ref<Mesh>> m_Meshes;

            MaterialGroup() = default;
            MaterialGroup(Ref<Mesh> mesh)
            {
                m_Meshes.insert(mesh);
            }
        };
        static std::unordered_map<Ref<Material>, MaterialGroup> m_MaterialGroups; // TODO: change to a type of weak ptr  
    private:
        static const uint32_t MAX_MESHES = 1000;
        static uint32_t m_MeshCount;
        static SceneData m_SceneData;
    };
}

#endif
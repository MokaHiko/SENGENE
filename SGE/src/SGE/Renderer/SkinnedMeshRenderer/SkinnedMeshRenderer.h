#ifndef SKINNED_MESH_RENDERER_H
#define SKINNED_MESH_RENDERER_H

#pragma once
#include <glad/glad.h>

#include "Core/Core.h"
#include "Renderer/Shader.h"
#include "Renderer/Model.h"

#include "Scene/Scene.h"
namespace SGE{
    class SkinnedMeshRenderer
    {
    public:
        SkinnedMeshRenderer();
        ~SkinnedMeshRenderer();

        static void Init();

        static void Configure(SceneData& sceneData);
        static void Begin();
        static void End();

        static void OnWindowResize(uint32_t width, uint32_t height);
    public:
	    static void Draw(Ref<AnimatedModel> model, const glm::vec3& position = glm::vec3(1.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
    private:
        static std::unordered_set <Ref<AnimatedModel>> m_Models;
        static SceneData m_SceneData;
        static Ref<Shader> m_Shader;
    };
}

#endif
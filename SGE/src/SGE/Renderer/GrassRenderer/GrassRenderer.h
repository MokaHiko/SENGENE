#ifndef GRASSRENDERER_H
#define GRASSRENDERER_H

#pragma once

#include <glad/glad.h>

#include "Core/Core.h"
#include "Renderer/Shader.h"
#include "Renderer/Model.h"

#include "Scene/Scene.h"

namespace SGE
{
    class GrassRenderer
    {
    public:
        GrassRenderer();
        ~GrassRenderer();

        static void Init(Ref<Model> grassModel, Ref<Shader> grassShader = nullptr);

        static void Configure(SceneData &sceneData);
        static void Begin();
        static void End();

        static void OnWindowResize(uint32_t width, uint32_t height);

    public:
        static void AddInstance(const glm::vec3 &position = glm::vec3(1.0f), const glm::vec3 &rotation = glm::vec3(0.0f), const glm::vec3 &scale = glm::vec3(1.0f));
        static SceneData GetSceneData() { return m_SceneData; };

    private:
        static Ref<Model> m_GrassModel;
        static SceneData m_SceneData;
        static Ref<Shader> m_Shader;
    };
}

#endif
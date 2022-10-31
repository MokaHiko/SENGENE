#ifndef SCENESERIALIZER_H
#define SCENESERIALIZER_H

#pragma once

#include "Scene.h"
#include <yaml-cpp/yaml.h>

namespace SGE {
    class SceneSerializer
    {
    public:
        SceneSerializer(const Ref<Scene>& scene);
        ~SceneSerializer();

        void Serialize(const std::string& filePath);
        void Deserialize(const std::string& filePath);

        static void SerializeEntity(YAML::Emitter& out, Entity entity);
    private:
        Ref<Scene> m_Scene;
    };
}

#endif
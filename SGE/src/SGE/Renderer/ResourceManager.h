#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#pragma once
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"

namespace SGE {
    class ResourceManager
    {
    public:
	    static Ref<Shader> CreateShader(const std::string& vertexPath, const std::string& fragmentPath);
	    static Ref<Shader> GetShader(const std::string& shaderName);

	    static Ref<Texture2D> CreateTexture(const std::string& texturePath);
	    static Ref<Texture2D> GetTexture(const std::string& textureName);

        static Ref<Material> CreateMaterial(const std::string& name, const glm::vec3& ambientColor = glm::vec3(0.0f), const glm::vec3 diffuseColor = glm::vec3(0.0f), 
                                            const Ref<Texture2D>& diffuseTexture = nullptr, const Ref<Texture2D>& specularTexture = nullptr);
        static Ref<Material> GetMaterial(const std::string& name);
    private:
        static std::unordered_map<std::string, Ref<Shader>> m_Shaders;
        static std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
        static std::unordered_map<std::string, Ref<Mesh>> m_Meshes;
        static std::unordered_map<std::string, Ref<Material>> m_Materials;
    };
}

#endif
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

	    static Ref<Mesh> CreateMesh(const std::string& meshPath);
	    static Ref<Mesh> GetMesh(const std::string& meshName);
    private:
        static std::unordered_map<std::string, Ref<Shader>> m_Shaders;
        static std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
        static std::unordered_map<std::string, Ref<Mesh>> m_Meshes;
    };
}

#endif
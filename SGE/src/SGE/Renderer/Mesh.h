#ifndef MESH_H
#define MESH_H

#pragma once
#include "Core/Core.h"
#include "Renderer/Texture.h"
#include <glm/glm.hpp>

namespace SGE {
    struct Vertex 
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    };

    struct Material
    {
        Material() {}
        Material(Ref<Texture2D> diffuseTexture, Ref<Texture2D> specularTexture)
        {
            if(diffuseTexture != nullptr)
                DiffuseTextureID = diffuseTexture->GetID();
            if(specularTexture != nullptr)
                SpecularTextureID = specularTexture->GetID();
        }
        
        void ChangeDiffuse(Ref<Texture2D> diffuseTexture)
        {
            DiffuseTextureID = diffuseTexture->GetID();
        }

        void ChangeSpecular(Ref<Texture2D> specularTexture)
        {
            SpecularTextureID = specularTexture->GetID();
        }

        static Ref<Material> CreateMaterial(Ref<Texture2D> diffuseTexture = nullptr, Ref<Texture2D> specularTexture = nullptr)
        {
            return CreateRef<Material>(diffuseTexture, specularTexture);
        }

        uint32_t DiffuseTextureID = 0;
        uint32_t SpecularTextureID = 0;

        float Shininess = 32;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();

        static Ref<Mesh> CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices = {});

        uint32_t VertexCount() const{ return m_VertexCount; }
        uint32_t m_VAO = 0, m_PositionBuffer = 0, m_TextureUnitBuffer = 0;

        void ChangeMaterial(uint32_t diffuseIndex, uint32_t specularIndex);
        void AddInstance(const glm::vec3& position = glm::vec3(1.0f), 
                         const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
        void Draw();

        uint32_t GetID() {return m_VAO;}; // TODO: create mesh UUIDs
    private:
        void ProcessMesh();
    private:
        std::vector<uint32_t> m_Indices;

        static const uint32_t MAX_INSTANCES = 100000;
        uint32_t m_InstanceCount = 0;
        uint32_t m_PositionBufferPointer = 0;
        uint32_t m_VertexCount = 0;
    };
}

#endif
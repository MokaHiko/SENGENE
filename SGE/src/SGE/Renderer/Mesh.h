#ifndef MESH_H
#define MESH_H

#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#include "Renderer/Texture.h"

namespace SGE {
    struct Mesh
    {
    public:
        Mesh() : m_NumIndices(0), m_BaseVertex(0), m_BaseIndex(0), m_MaterialIndex(0), m_NumBones(0) {}
        ~Mesh(){}

        uint32_t NumIndices() const {return m_NumIndices;}
        uint32_t BaseVertex() const {return m_BaseVertex;}
        uint32_t BaseIndex() const {return m_BaseIndex;}
        uint32_t MaterialIndex() const {return m_MaterialIndex;}
    private:
        uint32_t m_NumIndices;
        uint32_t m_BaseVertex;
        uint32_t m_BaseIndex;
        uint32_t m_MaterialIndex;
        uint32_t m_NumBones;

        friend class Model;
        friend class AnimatedModel;
    };

    struct Material
    {
        std::string Name;

        glm::vec3 AmbientColor;
        glm::vec3 DiffuseColor;
        glm::vec3 SpecularColor;

        Ref<Texture2D> DiffuseTexture;
        Ref<Texture2D> SpecularTexture;

        static Ref<Material> CreateMaterial(const std::string& name, const glm::vec3& ambientColor = glm::vec3(0.0f), const glm::vec3 diffuseColor = glm::vec3(0.0f), 
                                     const Ref<Texture2D>& diffuseTexture = nullptr, const Ref<Texture2D>& specularTexture = nullptr);

        Material() : AmbientColor(glm::vec3{0}), DiffuseColor(glm::vec3{0}), SpecularColor(0.0), DiffuseTexture(nullptr), SpecularTexture(nullptr), Name("Uknown Material") {}
    };
}

#endif
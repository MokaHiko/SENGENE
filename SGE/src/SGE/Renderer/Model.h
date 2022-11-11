#ifndef MODEL_H
#define MODEL_H

#pragma once

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Core/Core.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"

namespace SGE {
    class Model
    {
    private:
        enum BUFFER_TYPE
        {
            POSITION_VB = 0,
            INDEX_BUFFER = 1,
            TEXCOORD_VB = 2,
            NORMAL_VB = 3,

            NUM_BUFFERS = 4
        };
    public:
        Model(const std::string& modelPath, bool flipUVS = false);
        ~Model();

        static Ref<Model> CreateModel(const std::string& modelPath, bool flipUVS = false);

        // - Rendering
        void AddInstance(const glm::vec3& position = glm::vec3{1.0}, const glm::vec3& rotation = glm::vec3{0.0f}, const glm::vec3& scale = glm::vec3{1.0f});
        void Render(const Ref<Shader> shader);
	    void DrawMesh(const Mesh& mesh);
        void Clear();

        // - Panel Interface
        const std::vector<Ref<Material>>& GetMaterials() const {return m_Materials;}
        uint32_t GetNMaterials() const {return static_cast<uint32_t>(m_Materials.size());}
        uint32_t GetNMeshes() const {return static_cast<uint32_t>(m_Meshes.size());}
    private:
        // - Model Loading
        void LoadModel(const std::string& fileName, bool flipUVS);
        bool ProcessScene(const aiScene* scene, const std::string& fileName);
        bool ProcessMaterials(const aiScene* scene, const std::string& fileName);
        void ProcessMesh(const aiMesh* pMesh);
	    void ProcessNodeHierarchy(const aiNode* pNode, const glm::mat4& parentTransform, float timeInTicks);

        // - Buffers
        void PopulateBuffers();
    private:
        // Assimp Structures
        Assimp::Importer m_Importer{};
        const aiScene* m_aiScene = nullptr;

        // Model Structures
        std::vector<Mesh> m_Meshes{};
        std::vector<Ref<Material>>  m_Materials{};

        // Local Model Vertex Buffers (each mesh)
        std::vector<glm::vec3> m_Positions{};
        std::vector<glm::vec3> m_Normals{};
        std::vector<glm::vec2> m_TexCoords{};

        // Local Model Transform Buffers (each instance)
        uint32_t m_ModelTransformMatrixBuffer;

        // Local Model Index Buffer
        std::vector<uint32_t> m_Indices{};

        // GPU Buffer Handles
        std::vector<uint32_t> m_Buffers{};
    private:
        // Renderer Config
        uint32_t m_MaxInstances = 1000;
        uint32_t m_NumInstances = 0;

        // Model RendererID
        uint32_t m_RendererID = 0;
    };
}

#endif
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
            INDEX_BUFFER = 0,
            POSITION_VB = 1,
            TEXCOORD_VB = 2,
            NORMAL_VB = 3,

            NUM_BUFFERS = 4
        };

    public:
        Model(const std::string& modelPath, bool flipUVS = false);
        ~Model();

        static Ref<Model> CreateModel(const std::string& modelPath, bool flipUVS = false);
        void AddInstance(const glm::vec3& position = glm::vec3{1.0}, const glm::vec3& rotation = glm::vec3{0.0f}, const glm::vec3& scale = glm::vec3{1.0f});
        void Render(const Ref<Shader> shader);

        void Clear();
	    void DrawMesh(const Mesh& mesh);

        const std::vector<Ref<Material>>& GetMaterials() const {return m_Materials;}
        
        uint32_t GetNMaterials() const {return m_Materials.size();}
        uint32_t GetNMeshes() const {return m_Meshes.size();}
    private:
        void LoadModel(const std::string& fileName, bool flipUVS);

        bool ParseScene(const aiScene* scene, const std::string& fileName);
        bool ProcessMaterials(const aiScene* scene, const std::string& fileName);
        void ProcessMesh(const aiMesh* aiMesh);

        std::vector<Mesh> m_Meshes;
        std::vector<Ref<Material>>  m_Materials;
        void PopulateBuffers();
    private:
        // mesh property buffers
        std::vector<glm::vec3> m_Positions;
        std::vector<glm::vec3> m_Normals;
        std::vector<glm::vec2> m_TexCoords;

        std::vector<uint32_t> m_Indices;

        std::vector<uint32_t> m_Buffers;
    private:
        uint32_t m_MaxInstances = 10000;
        uint32_t m_NumInstances = 0;

        // model buffers
        uint32_t m_ModelTransformMatrixBuffer;
        uint32_t m_RendererID;
    };
}

#endif
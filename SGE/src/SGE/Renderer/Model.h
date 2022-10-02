#ifndef MODEL_H
#define MODEL_H

#pragma once

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Core/Core.h"
#include "Renderer/Texture.h"

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
        struct Mesh
        {
        public:
            Mesh() : m_NumIndices(0), m_BaseVertex(0), m_BaseIndex(0), m_MaterialIndex(0) {}
            ~Mesh(){}
        private:
            uint32_t m_NumIndices;
            uint32_t m_BaseVertex;
            uint32_t m_BaseIndex;
            uint32_t m_MaterialIndex;

            friend class Model;
        };
    public:
        Model(const std::string& modelPath);
        ~Model();

        static Ref<Model> CreateModel(const std::string& modelPath);
        void Render();

        void Clear();
    private:
        void LoadModel(const std::string& fileName);

        bool ParseScene(const aiScene* scene, const std::string& fileName);
        bool ProcessMaterials(const aiScene* scene, const std::string& fileName);
        void ProcessMesh(const aiMesh* aiMesh);

        std::vector<Mesh> m_Meshes;
        std::vector<Ref<Texture2D>> m_Textures;
        void PopulateBuffers();
    private:
        // Model vertex buffers
        std::vector<glm::vec3> m_Positions;
        std::vector<glm::vec3> m_Normals;
        std::vector<glm::vec2> m_TexCoords;

        std::vector<uint32_t> m_Indices;

        std::vector<uint32_t> m_Buffers;

        uint32_t m_RendererID;
    };
}

#endif
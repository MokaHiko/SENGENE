#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H

#pragma once

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Core/Core.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Core/TimeStep.h"

namespace SGE {
    static const uint32_t MAX_NUM_BONES_PER_VERTEX = 4;

    struct BoneInfo
    {
        BoneInfo(const glm::mat4& offset)
        {
            OffsetMatrix = offset;
            FinalTransformationMatrix = glm::mat4(1.0f);
        }

        glm::mat4 OffsetMatrix{ 1.0f };                 // local to bone space
        glm::mat4 FinalTransformationMatrix{ 1.0f };    // local space matrix after animation
    };

    struct VertexBoneData 
    {
        VertexBoneData() {}

        void AddBoneData(int boneID, float weight)
        {
            for(int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
            {
                if(Weights[i] == 0.0f) 
                {
                    BoneIDS[i] = boneID;
                    Weights[i] = weight;
                    return;
                }
            }
            assert(0);  // Should Never Reach More than Alloted Space for Bones
        }

        int BoneIDS[MAX_NUM_BONES_PER_VERTEX] = {};
        float Weights[MAX_NUM_BONES_PER_VERTEX] = {};
    };

    class AnimatedModel
    {
    private:
        enum BUFFER_TYPE
        {
            POSITION_VB = 0,
            INDEX_BUFFER = 1,
            TEXCOORD_VB = 2,
            NORMAL_VB = 3,
            BONE_VB = 4,

            NUM_BUFFERS = 5
        };
    public:
        AnimatedModel(const std::string& modelPath, bool flipUVS = false);
        ~AnimatedModel();

        static Ref<AnimatedModel> CreateAnimatedModel(const std::string& modelPath, bool flipUVS = false);

        // - Rendering
        void AddInstance(const glm::vec3& position = glm::vec3{1.0}, const glm::vec3& rotation = glm::vec3{0.0f}, const glm::vec3& scale = glm::vec3{1.0f});
        void Render(const Ref<Shader> shader);
	    void DrawMesh(const Mesh& mesh);
        void Clear();

        // - Panel Interface
        const std::vector<Ref<Material>>& GetMaterials() const {return m_Materials;}
        uint32_t GetNMaterials() const {return static_cast<uint32_t>(m_Materials.size());}
        uint32_t GetNMeshes() const {return static_cast<uint32_t>(m_Meshes.size());}
        uint32_t GetNBones() const {return static_cast<uint32_t>(m_Bones.size());}
        const glm::mat4& GetRootBoneTransform() const {return m_RootBoneTransform;}
    private:
        // - AnimatedModel Loading
        void LoadAnimatedModel(const std::string& fileName, bool flipUVS);
        bool ProcessScene(const aiScene* scene, const std::string& fileName);
        bool ProcessMaterials(const aiScene* scene, const std::string& fileName);
        void ProcessMesh(const aiMesh* pMesh);
	    void ProcessNodeHierarchy(const aiNode* pNode, const glm::mat4& parentTransform, float timeInTicks);

        // - Animation
        void ProcessMeshBones(uint32_t meshIndex, const aiMesh* pMesh);
        void ProcessSingleBone(uint32_t meshIndex, aiBone* pBone);
        int GetBoneID(const aiBone* pBone);
        const aiNodeAnim* GetNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName);
        void GetBoneTransforms(std::vector<glm::mat4>& Transforms, float animationTime);

        void InterpolateScale(glm::vec3& scale, float animationTimeTicks, const aiNodeAnim* pNodeAnim);
        uint32_t FindScaling(float animationTimeTicks, const aiNodeAnim* pNodeAnim);
        void InterpolateRotation(aiQuaternion& rotation, float animationTimeTicks, const aiNodeAnim* pNodeAnim);
        uint32_t FindRotation(float animationTimeTicks, const aiNodeAnim* pNodeAnim);
        void InterpolateTranslation(glm::vec3& translation, float animationTimeTicks, const aiNodeAnim* pNodeAnim);
        uint32_t FindTranslation(float animationTimeTicks, const aiNodeAnim* pNodeAnim);

        // - Helper
        const glm::mat4 AssimpToGlmMatrix(const aiMatrix4x4& matrix);

        // - Buffers
        void PopulateBuffers();
    private:
        // Assimp Structures
        Assimp::Importer m_Importer{};
        const aiScene* m_aiScene = nullptr;

        // AnimatedModel Structures
        std::vector<Mesh> m_Meshes{};
        std::vector<Ref<Material>>  m_Materials{};

        // Bone Structures
        std::vector<glm::mat4> m_BoneTransforms{};
        std::vector<VertexBoneData> m_Bones{};
        std::vector<BoneInfo> m_BoneInfos{};
        std::map<std::string, uint32_t> m_BoneNamesToIndex{};
        glm::mat4 m_GlobalInverseTransform{ 1.0f };
        glm::mat4 m_RootBoneTransform{1.0f};
        bool m_AnimatedInPlace = false;

        // Local AnimatedModel Vertex Buffers (each mesh)
        std::vector<glm::vec3> m_Positions{};
        std::vector<glm::vec3> m_Normals{};
        std::vector<glm::vec2> m_TexCoords{};

        // Local AnimatedModel Transform Buffers (each instance)
        uint32_t m_AnimatedModelTransformMatrixBuffer;

        // Local AnimatedModel Index Buffer
        std::vector<uint32_t> m_Indices{};

        // GPU Buffer Handles
        std::vector<uint32_t> m_Buffers{};
    private:
        // Renderer Config
        uint32_t m_MaxInstances = 1000;
        uint32_t m_NumInstances = 0;

        // AnimatedModel RendererID
        uint32_t m_RendererID = 0;
    };
}

#endif
#include "AnimatedModel.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Renderer/ResourceManager.h"

// TODO: REMOVE
#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>

static const int POSITION_LOCATION = 0;
static const int NORMAL_LOCATION = 1;
static const int TEX_COORD_LOCATION = 2;
static const int BONE_ID_LOCATION = 3;
static const int BONE_WEIGHT_LOCATION = 4;
static const int TRANSFORM_MATRIX_LOCATION = 5;

namespace SGE
{
	AnimatedModel::AnimatedModel(const std::string &modelPath, bool flipUVS)
		: m_RendererID(0), m_aiScene(nullptr)
	{
		// Generate AnimatedModel rendererID
		glGenVertexArrays(1, &m_RendererID);

		// Generate AnimatedModel Vertex & Index Buffers
		m_Buffers.resize(BUFFER_TYPE::NUM_BUFFERS);
		for (uint32_t i = 0; i < m_Buffers.size(); i++)
			glGenBuffers(1, &m_Buffers[i]);

		printf("MODEL::LOADING %s ==> ", modelPath.c_str());
		printf(" - flipUVS: %s\n", flipUVS ? "True" : "False");
		LoadAnimatedModel(modelPath, flipUVS);
	}

	AnimatedModel::~AnimatedModel()
	{
		// delete m_aiScene; TODO: Clean Scene
		for (auto buffer : m_Buffers)
			glDeleteBuffers(1, &buffer);

		glDeleteBuffers(1, &m_AnimatedModelTransformMatrixBuffer);
		glDeleteVertexArrays(1, &m_RendererID);
	}

	Ref<AnimatedModel> AnimatedModel::CreateAnimatedModel(const std::string &modelPath, bool flipUVS)
	{
		return ResourceManager::CreateAnimatedModel(modelPath, flipUVS);
	}

	void AnimatedModel::AddInstance(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
	{
		// TODO: ROTATE AROUND ANY AXIS
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		model *= glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
		model = glm::scale(model, scale);

		glBindBuffer(GL_ARRAY_BUFFER, m_AnimatedModelTransformMatrixBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, m_NumInstances * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(model));
		m_NumInstances++;
	}

	void AnimatedModel::LoadAnimatedModel(const std::string &fileName, bool flipUVS)
	{
		glBindVertexArray(m_RendererID);
		bool success = false;

		uint32_t ASSIMP_IMPORT_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals |
									   aiProcess_JoinIdenticalVertices;
		ASSIMP_IMPORT_FLAGS |= flipUVS ? aiProcess_FlipUVs : 0;

		m_aiScene = m_Importer.ReadFile(fileName.c_str(), ASSIMP_IMPORT_FLAGS);
		if (m_aiScene == nullptr)
		{
			std::cout << "Failed to load model at " << fileName << "\n";
			return;
		}

		m_GlobalInverseTransform = AssimpToGlmMatrix(m_aiScene->mRootNode->mTransformation.Inverse());
		success = ProcessScene(m_aiScene, fileName);
		success = ProcessMaterials(m_aiScene, fileName);

		if (!success)
		{
			std::cout << "Failed to parse model " << fileName << "\n";
			return;
		}

		// Clear Local Buffers
		Clear();
		glBindVertexArray(0);
	}

	static float startTime = (float)glfwGetTime();
	void AnimatedModel::Render(const Ref<Shader> shader)
	{
		// Process Animation Transforms
		float animationTime = ((float)glfwGetTime() - startTime); // in seconds

		// Check if AnimatedModel Has Animation Transforms
		if (m_aiScene->HasAnimations())
		{
			GetBoneTransforms(m_BoneTransforms, animationTime);
			shader->SetMat4Array("u_Bones", m_BoneTransforms);
		}

		glBindVertexArray(m_RendererID);
		for (uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			// Bind Material Properties
			uint32_t materialIndex = m_Meshes[i].m_MaterialIndex;
			assert(materialIndex < m_Materials.size());

			if (m_Materials[materialIndex])
			{
				shader->SetVec3("u_Material.Ambient", m_Materials[materialIndex]->AmbientColor);
				shader->SetVec3("u_Material.Diffuse", m_Materials[materialIndex]->DiffuseColor);
				shader->SetVec3("u_Material.Specular", m_Materials[materialIndex]->SpecularColor);
			}

			// Bind Material Textures
			if (m_Materials[materialIndex]->DiffuseTexture)
			{
				shader->SetBool("u_Material.HasDiffuseTexture", true);
				m_Materials[materialIndex]->DiffuseTexture->Bind(0);
			}

			if (m_Materials[materialIndex]->SpecularTexture)
			{
				shader->SetBool("u_Material.HasSpecularTexture", true);
				m_Materials[materialIndex]->SpecularTexture->Bind(1);
			}

			// Draw Call
			DrawMesh(m_Meshes[i]);

			// Unbind Material Textures/Properties
			if (m_Materials[materialIndex]->DiffuseTexture)
				shader->SetBool("u_Material.HasDiffuseTexture", false);
			if (m_Materials[materialIndex]->SpecularTexture)
				shader->SetBool("u_Material.HasSpecularTexture", false);
		}

		m_NumInstances = 0;
	}

	void AnimatedModel::DrawMesh(const Mesh &mesh)
	{
		// draw mesh
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES,									 // Primitive to Render
										  mesh.NumIndices(),							 // Number of Elements (Indices) In Mesh
										  GL_UNSIGNED_INT,								 // Type Of Indices Data
										  (void *)(sizeof(uint32_t) * mesh.BaseIndex()), // Offset of Starting Index in (ElementArray) Index Buffer
										  m_NumInstances,								 // Number of Instances of this Geometry to Render
										  mesh.BaseVertex());							 // Constant to be Added to Each Vertex Array Buffer (i.e the value of the vertex attribs)
	}

	bool AnimatedModel::ProcessScene(const aiScene *scene, const std::string &fileName)
	{
		// Resize Meshes and Materials
		m_Meshes.resize(scene->mNumMeshes);
		m_Materials.resize(scene->mNumMaterials);

		uint32_t nVertices = 0;
		uint32_t nIndices = 0;

		// Assign Mesh Properties & Allocate Space for Buffers
		for (uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			m_Meshes[i].m_MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
			m_Meshes[i].m_NumIndices = scene->mMeshes[i]->mNumFaces * 3;
			m_Meshes[i].m_BaseVertex = nVertices;
			m_Meshes[i].m_BaseIndex = nIndices;
			m_Meshes[i].m_NumBones = scene->mMeshes[i]->mNumBones;

			nVertices += scene->mMeshes[i]->mNumVertices;
			nIndices += m_Meshes[i].m_NumIndices;
		}

		// Resize Bones Mapping to Fit EFfected Vertices
		m_Bones.resize(nVertices);

		// Reserve local Buffers to Fit All Mesh Vertex Data
		m_Positions.reserve(nVertices);
		m_Normals.reserve(nVertices);
		m_TexCoords.reserve(nVertices);
		m_Indices.reserve(nIndices);

		// Populate Local Buffers with Mesh Data
		for (uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			const aiMesh *aiMesh = scene->mMeshes[i];
			ProcessMesh(aiMesh);

			if (scene->mMeshes[i]->HasBones())
				ProcessMeshBones(i, scene->mMeshes[i]);
		}

		// Populate GPU Buffers with Local Buffer Data
		PopulateBuffers();
		return true;
	}

	bool AnimatedModel::ProcessMaterials(const aiScene *scene, const std::string &fileName)
	{
		bool success = true;

		std::size_t slashIndex = fileName.find_last_of("/");
		std::string dir;

		if (slashIndex == std::string::npos)
			dir = ".";
		else if (slashIndex == 0)
			dir = "/";
		else
			dir = fileName.substr(0, slashIndex);

		for (uint32_t i = 0; i < scene->mNumMaterials; i++)
		{
			const aiMaterial *aiMaterial = scene->mMaterials[i];
			m_Materials[i] = Material::CreateMaterial(scene->mMaterials[i]->GetName().data);

			if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString pathBuffer;
				if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &pathBuffer, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					std::string texturePath(pathBuffer.data);

					if (texturePath.substr(0, 2) == ".\\")
						texturePath = texturePath.substr(2, texturePath.size() - 2);
					std::string fullPath = dir + "/" + texturePath;

					// Check if Texture is Embedded
					const aiTexture *pAiEmbeddedTexture = m_aiScene->GetEmbeddedTexture(pathBuffer.C_Str());
					if (pAiEmbeddedTexture)
					{
						m_Materials[i]->DiffuseTexture = Texture2D::CreateTexture2D(fullPath, pAiEmbeddedTexture->pcData, pAiEmbeddedTexture->mWidth);
					}
					else
					{
						m_Materials[i]->DiffuseTexture = Texture2D::CreateTexture2D(fullPath);
					}

					if (!m_Materials[i]->DiffuseTexture)
					{
						success = false;
					}
				}
			}

			if (aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				aiString pathBuffer;
				if (aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &pathBuffer, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					std::string texturePath(pathBuffer.data);

					if (texturePath.substr(0, 2) == ".\\")
						texturePath = texturePath.substr(2, texturePath.size() - 2);
					std::string fullPath = dir + "/" + texturePath;

					const aiTexture *pAiEmbeddedTexture = m_aiScene->GetEmbeddedTexture(pathBuffer.C_Str());

					if (pAiEmbeddedTexture)
					{
						m_Materials[i]->SpecularTexture = Texture2D::CreateTexture2D(fullPath, pAiEmbeddedTexture->pcData, pAiEmbeddedTexture->mWidth);
					}
					else
					{
						m_Materials[i]->SpecularTexture = Texture2D::CreateTexture2D(fullPath);
					}

					if (!m_Materials[i]->SpecularTexture)
						success = false;
				}
			}

			// Ambient
			aiColor3D AmbientColor(0.0);
			if (aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == AI_SUCCESS)
				m_Materials[i]->AmbientColor = {AmbientColor.r, AmbientColor.g, AmbientColor.b};

			// Diffuse
			aiColor3D DiffuseColor(0.0);
			if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS)
				m_Materials[i]->DiffuseColor = {DiffuseColor.r, DiffuseColor.g, DiffuseColor.b};

			// TODO: Specular
			aiColor3D SpecularColor(0.0f);
			if (aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == AI_SUCCESS)
				m_Materials[i]->SpecularColor = {SpecularColor.r, SpecularColor.g, SpecularColor.b};

			// TODO: Specular Intensity (Shininess)
		}

		return success;
	}

	void AnimatedModel::ProcessMesh(const aiMesh *pMesh)
	{
		// Populate Local Buffers with Vertex Data
		for (uint32_t i = 0; i < pMesh->mNumVertices; i++)
		{
			const aiVector3D &position = pMesh->mVertices[i];
			const aiVector3D &normal = pMesh->mNormals[i];
			const aiVector3D &texCoord = pMesh->HasTextureCoords(0) ? pMesh->mTextureCoords[0][i] : aiVector3D(0.0f);

			m_Positions.push_back({position.x, position.y, position.z});
			m_Normals.push_back({normal.x, normal.y, normal.z});
			m_TexCoords.push_back({texCoord.x, texCoord.y});
		}

		// Populate Local Index Buffer with Index Data
		for (uint32_t i = 0; i < pMesh->mNumFaces; i++)
		{
			const aiFace &face = pMesh->mFaces[i];

			// Faces Were Specified to be Triangulated
			assert(face.mNumIndices == 3);

			m_Indices.push_back(face.mIndices[0]);
			m_Indices.push_back(face.mIndices[1]);
			m_Indices.push_back(face.mIndices[2]);
		}
	}

	void AnimatedModel::ProcessNodeHierarchy(const aiNode *pNode, const glm::mat4 &parentTransform, float timeInTicks)
	{
		std::string nodeName(pNode->mName.data);

		// Get Desired Animation
		const aiAnimation *pAnimation = m_aiScene->mAnimations[0];

		// NodeTransformationMatrix Converts from Child to Parent Coordinate Systems
		glm::mat4 nodeTransformationMatrix = AssimpToGlmMatrix(pNode->mTransformation);

		// Get Ai Node Anim
		const aiNodeAnim *pNodeAnim = GetNodeAnim(pAnimation, nodeName);

		// Replace Transformation Matrix of Animation if pNodeAnim was Found
		if (pNodeAnim)
		{
			glm::vec3 scale;
			InterpolateScale(scale, timeInTicks, pNodeAnim);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

			aiQuaternion rotationQ;
			InterpolateRotation(rotationQ, timeInTicks, pNodeAnim);
			glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotationQ.w, rotationQ.x, rotationQ.y, rotationQ.z));

			glm::vec3 translation;
			InterpolateTranslation(translation, timeInTicks, pNodeAnim);
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

			nodeTransformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		}

		// Assign Intermediate Transform Matrix from
		glm::mat4 globalTransformation = parentTransform * nodeTransformationMatrix;
		if (m_BoneNamesToIndex.find(nodeName) != m_BoneNamesToIndex.end())
		{
			uint32_t boneIndex = m_BoneNamesToIndex[nodeName];
			m_BoneInfos[boneIndex].FinalTransformationMatrix = m_GlobalInverseTransform * globalTransformation * m_BoneInfos[boneIndex].OffsetMatrix;
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			ProcessNodeHierarchy(pNode->mChildren[i], globalTransformation, timeInTicks);
	}

	void AnimatedModel::ProcessMeshBones(uint32_t meshIndex, const aiMesh *pMesh)
	{
		// Process Each Bone In Mesh
		for (uint32_t i = 0; i < pMesh->mNumBones; i++)
			ProcessSingleBone(meshIndex, pMesh->mBones[i]);
	}

	void AnimatedModel::ProcessSingleBone(uint32_t meshIndex, aiBone *pBone)
	{
		// BoneIDS correspond to boneInfos Index
		uint32_t boneID = GetBoneID(pBone);

		// Check If new Bone
		if (boneID == m_BoneInfos.size())
		{
			// Store new offsetMatrix to boneInfo
			BoneInfo boneInfo(AssimpToGlmMatrix(pBone->mOffsetMatrix));
			m_BoneInfos.push_back(boneInfo);
		}

		for (uint32_t i = 0; i < pBone->mNumWeights; i++)
		{
			const aiVertexWeight &vw = pBone->mWeights[i];

			// Generate Global VertexID (as vertex weight vertex IDs are relative to 0)
			uint32_t globalVertexID = m_Meshes[meshIndex].BaseVertex() + vw.mVertexId;

			assert(globalVertexID < m_Bones.size());
			m_Bones[globalVertexID].AddBoneData(boneID, vw.mWeight);
		}
	}

	int AnimatedModel::GetBoneID(const aiBone *pBone)
	{
		int boneID = 0;
		std::string boneName = pBone->mName.C_Str();

		if (m_BoneNamesToIndex.find(boneName) == m_BoneNamesToIndex.end())
		{
			// Assign New Bone Name to an ID
			boneID = (int)m_BoneNamesToIndex.size();
			m_BoneNamesToIndex[boneName] = boneID;
		}
		else
		{
			boneID = m_BoneNamesToIndex[boneName];
		}

		return boneID;
	}

	void AnimatedModel::GetBoneTransforms(std::vector<glm::mat4> &Transforms, float animationTime)
	{
		// Calculate animation duration
		float ticksPerSecond = static_cast<float>(m_aiScene->mAnimations[0]->mTicksPerSecond) != 0.0f ? m_aiScene->mAnimations[0]->mTicksPerSecond : 25.0f;
		float timeInTicks = animationTime * ticksPerSecond;
		float animationTimeTicks = fmod(timeInTicks, (float)m_aiScene->mAnimations[0]->mDuration);

		// Process Hierarchy to get Final Transformation
		glm::mat4 identity{1.0f};
		ProcessNodeHierarchy(m_aiScene->mRootNode, identity, animationTimeTicks);

		// Resize to How Many Bones/BoneInfos is Present in AnimatedModel
		Transforms.resize(m_BoneInfos.size());

		// Store Root Bone Transform
		assert(m_BoneInfos.size() > 0);
		m_RootBoneTransform = m_BoneInfos[0].FinalTransformationMatrix;

		// Store All Final Transformations in Passed Transform Vector
		if (!m_AnimatedInPlace)
		{
			for (uint32_t i = 0; i < m_BoneInfos.size(); i++)
			{
				Transforms[i] = m_BoneInfos[i].FinalTransformationMatrix;
			}
		}
		else
		{
			glm::mat4 inPlaceTransform = glm::inverse(m_RootBoneTransform);
			for (uint32_t i = 0; i < m_BoneInfos.size(); i++)
			{
				Transforms[i] = inPlaceTransform * m_BoneInfos[i].FinalTransformationMatrix;
			}
		}
	}

	const aiNodeAnim *AnimatedModel::GetNodeAnim(const aiAnimation *pAnimation, const std::string &nodeName)
	{
		for (uint32_t i = 0; i < pAnimation->mNumChannels; i++)
		{
			const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];
			if (std::string(pNodeAnim->mNodeName.data) == nodeName)
				return pNodeAnim;
		}

		return nullptr;
	}

	void AnimatedModel::InterpolateScale(glm::vec3 &scale, float animationTimeTicks, const aiNodeAnim *pNodeAnim)
	{
		// You need atleast two values to interpolate
		if (pNodeAnim->mNumScalingKeys < 2)
		{
			aiVector3D scaleKeys = pNodeAnim->mScalingKeys[0].mValue;
			scale = glm::vec3{scaleKeys.x, scaleKeys.y, scaleKeys.z};
			return;
		}

		uint32_t scaleIndex = FindScaling(animationTimeTicks, pNodeAnim);
		uint32_t nextScaleIndex = scaleIndex + 1;

		assert(nextScaleIndex < pNodeAnim->mNumScalingKeys);

		float t1 = (float)pNodeAnim->mScalingKeys[scaleIndex].mTime;
		float t2 = (float)pNodeAnim->mScalingKeys[nextScaleIndex].mTime;
		float dt = t2 - t1;
		float factor = (animationTimeTicks - t1) / dt;

		assert(factor >= 0.0f && factor <= 1.0f);
		const aiVector3D &start = pNodeAnim->mScalingKeys[scaleIndex].mValue;
		const aiVector3D &end = pNodeAnim->mScalingKeys[nextScaleIndex].mValue;
		aiVector3D delta = end - start;
		scale = glm::vec3{start.x, start.y, start.z} + (factor * glm::vec3{delta.x, delta.y, delta.z});
	}

	uint32_t AnimatedModel::FindScaling(float animationTimeTicks, const aiNodeAnim *pNodeAnim)
	{
		assert(pNodeAnim->mNumScalingKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
		{
			float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;

			if (animationTimeTicks < t)
				return i;
		}
		return 0;
	}

	void AnimatedModel::InterpolateRotation(aiQuaternion &rotation, float animationTimeTicks, const aiNodeAnim *pNodeAnim)
	{
		// You need atleast two values to interpolate
		if (pNodeAnim->mNumRotationKeys < 2)
		{
			rotation = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		uint32_t rotationIndex = FindRotation(animationTimeTicks, pNodeAnim);
		uint32_t nextRotationIndex = rotationIndex + 1;

		assert(nextRotationIndex < pNodeAnim->mNumRotationKeys);

		float t1 = (float)pNodeAnim->mRotationKeys[rotationIndex].mTime;
		float t2 = (float)pNodeAnim->mRotationKeys[nextRotationIndex].mTime;
		float dt = t2 - t1;
		float factor = (animationTimeTicks - t1) / dt;

		assert(factor >= 0.0f && factor <= 1.0f);
		const aiQuaternion &start = pNodeAnim->mRotationKeys[rotationIndex].mValue;
		const aiQuaternion &end = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;
		aiQuaternion::Interpolate(rotation, start, end, factor);
		rotation = rotation.Normalize();
	}

	uint32_t AnimatedModel::FindRotation(float animationTimeTicks, const aiNodeAnim *pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
		{
			float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;

			if (animationTimeTicks < t)
				return i;
		}
		return 0;
	}

	void AnimatedModel::InterpolateTranslation(glm::vec3 &translation, float animationTimeTicks, const aiNodeAnim *pNodeAnim)
	{
		// You need atleast two values to interpolate
		if (pNodeAnim->mNumPositionKeys < 2)
		{
			aiVector3D translationKeys = pNodeAnim->mPositionKeys[0].mValue;
			translation = glm::vec3{translationKeys.x, translationKeys.y, translationKeys.z};
			return;
		}

		uint32_t translationIndex = FindTranslation(animationTimeTicks, pNodeAnim);
		uint32_t nextTranslationIndex = translationIndex + 1;

		assert(nextTranslationIndex < pNodeAnim->mNumPositionKeys);
		float t1 = (float)pNodeAnim->mPositionKeys[translationIndex].mTime;
		float t2 = (float)pNodeAnim->mPositionKeys[nextTranslationIndex].mTime;
		float dt = t2 - t1;
		float factor = (animationTimeTicks - t1) / dt;

		assert(factor >= 0.0f && factor <= 1.0f);
		const aiVector3D &start = pNodeAnim->mPositionKeys[translationIndex].mValue;
		const aiVector3D &end = pNodeAnim->mPositionKeys[nextTranslationIndex].mValue;
		aiVector3D delta = end - start;
		translation = glm::vec3{start.x, start.y, start.z} + (factor * glm::vec3{delta.x, delta.y, delta.z});
	}

	uint32_t AnimatedModel::FindTranslation(float animationTimeTicks, const aiNodeAnim *pNodeAnim)
	{
		assert(pNodeAnim->mNumPositionKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
		{
			float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;

			if (animationTimeTicks < t)
				return i;
		}
		return 0;
	}

	const glm::mat4 AnimatedModel::AssimpToGlmMatrix(const aiMatrix4x4 &matrix)
	{
		return glm::transpose(glm::make_mat4(&matrix.a1));
	}

	void AnimatedModel::PopulateBuffers()
	{
		// Fill Mesh Vertex Buffers
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POSITION_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), m_Positions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(POSITION_LOCATION);
		glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), m_Normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), m_TexCoords.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

		// Animation/Skinned Mesh Buffers
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), m_Bones.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(BONE_ID_LOCATION);
		glVertexAttribIPointer(BONE_ID_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (void *)0);
		glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
		glVertexAttribPointer(BONE_WEIGHT_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void *)(MAX_NUM_BONES_PER_VERTEX * sizeof(int32_t)));

		// Index Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);

		// Generate AnimatedModel Instanced Transform Matrix Buffer
		glm::mat4 *modelBuffer = new glm::mat4[m_MaxInstances];
		for (uint32_t i = 0; i < m_MaxInstances; i++)
			modelBuffer[i] = glm::mat4{1.0f};

		glGenBuffers(1, &m_AnimatedModelTransformMatrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_AnimatedModelTransformMatrixBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_MaxInstances, modelBuffer, GL_DYNAMIC_DRAW);

		// Make Transform Matrix Buffer Attrib Update Per Instance glVertexAttribDivisor(AttribLocation, 1)
		for (uint32_t i = 0; i < 4; i++)
		{
			glEnableVertexAttribArray(TRANSFORM_MATRIX_LOCATION + i);
			glVertexAttribPointer(TRANSFORM_MATRIX_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(TRANSFORM_MATRIX_LOCATION + i, 1);
		}

		delete[] modelBuffer;
	}

	void AnimatedModel::Clear()
	{
		// Clear Local Buffers
		m_Positions.clear();
		m_Normals.clear();
		m_Indices.clear();
		m_TexCoords.clear();

		// TODO: Clear GPU Buffers
	}
}
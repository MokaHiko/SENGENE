#include "Model.h"

#include <glad/glad.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "Renderer/ResourceManager.h"

static const int POSITION_LOCATION = 0;
static const int NORMAL_LOCATION = 1;
static const int TEX_COORD_LOCATION = 2;
static const int BONE_ID_LOCATION = 3;
static const int BONE_WEIGHT_LOCATION = 4;
static const int TRANSFORM_MATRIX_LOCATION = 5;

namespace SGE {
	Model::Model(const std::string& modelPath, bool flipUVS)
		:m_RendererID(0), m_aiScene(nullptr)
	{
		// Generate Model rendererID
		glGenVertexArrays(1, &m_RendererID);

		// Generate Model Vertex & Index Buffers
		m_Buffers.resize(BUFFER_TYPE::NUM_BUFFERS);
		for(uint32_t i = 0; i< m_Buffers.size(); i++)
			glGenBuffers(1, &m_Buffers[i]);

		printf("MODEL::LOADING %s ==> ", modelPath.c_str());
		LoadModel(modelPath, flipUVS);
		printf("SUCCESS\n");
	}

	Model::~Model()
	{
		//delete m_aiScene; TODO: Clean Scene
		for(auto buffer : m_Buffers)
			glDeleteBuffers(1, &buffer);
		
		glDeleteBuffers(1, &m_ModelTransformMatrixBuffer);
		glDeleteVertexArrays(1, &m_RendererID);
	}
	
	Ref<Model> Model::CreateModel(const std::string& modelPath, bool flipUVS)
	{
		return ResourceManager::CreateModel(modelPath, flipUVS);
	}
	
	void Model::AddInstance(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		// TODO: ROTATE AROUND ANY AXIS
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		model *= glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
		model = glm::scale(model, scale);

		glBindBuffer(GL_ARRAY_BUFFER, m_ModelTransformMatrixBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, m_NumInstances * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(model));
		m_NumInstances++;
	}

	void Model::LoadModel(const std::string& fileName, bool flipUVS)
	{
		glBindVertexArray(m_RendererID);
		bool success = false;

		uint32_t ASSIMP_IMPORT_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals | 
									   aiProcess_JoinIdenticalVertices;
		ASSIMP_IMPORT_FLAGS |= flipUVS ? aiProcess_FlipUVs : 0;

		m_aiScene = m_Importer.ReadFile(fileName.c_str(), ASSIMP_IMPORT_FLAGS);
		if (m_aiScene == nullptr) {
			std::cout << "Failed to load model at " << fileName << "\n";
			return;
		}

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
	
	void Model::Render(const Ref<Shader> shader)
	{
		glBindVertexArray(m_RendererID);
		for(uint32_t i = 0; i < m_Meshes.size(); i++)
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

	void Model::DrawMesh(const Mesh& mesh)
	{
		// draw mesh
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES,										// Primitive to Render 
										  mesh.NumIndices(), 								// Number of Elements (Indices) In Mesh
										  GL_UNSIGNED_INT,									// Type Of Indices Data
										  (void*)(sizeof(uint32_t) * mesh.BaseIndex()),		// Offset of Starting Index in (ElementArray) Index Buffer
										  m_NumInstances,									// Number of Instances of this Geometry to Render 
										  mesh.BaseVertex());								// Constant to be Added to Each Vertex Array Buffer (i.e the value of the vertex attribs)
	}

	bool Model::ProcessScene(const aiScene* scene, const std::string& fileName)
	{
		// Resize Meshes and Materials
		m_Meshes.resize(scene->mNumMeshes);
		m_Materials.resize(scene->mNumMaterials);

		uint32_t nVertices = 0;
		uint32_t nIndices = 0;

		// Assign Mesh Properties & Allocate Space for Buffers
		for(uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			m_Meshes[i].m_MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
			m_Meshes[i].m_NumIndices 	= scene->mMeshes[i]->mNumFaces * 3;
			m_Meshes[i].m_BaseVertex 	= nVertices;
			m_Meshes[i].m_BaseIndex  	= nIndices;
			m_Meshes[i].m_NumBones 		= scene->mMeshes[i]->mNumBones;

			nVertices += scene->mMeshes[i]->mNumVertices;
			nIndices  += m_Meshes[i].m_NumIndices;
		}
		// Reserve local Buffers to Fit All Mesh Vertex Data
		m_Positions.reserve(nVertices);
		m_Normals.reserve(nVertices);
		m_TexCoords.reserve(nVertices);
		m_Indices.reserve(nIndices);

		// Populate Local Buffers with Mesh Data
		for(uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			const aiMesh* aiMesh = scene->mMeshes[i];
			ProcessMesh(aiMesh);
		}

		// Populate GPU Buffers with Local Buffer Data
		PopulateBuffers();
		return true;
	}
	
	bool Model::ProcessMaterials(const aiScene* scene, const std::string& fileName)
	{
		bool success = true;

		std::size_t slashIndex = fileName.find_last_of("/");
		std::string dir;

		if(slashIndex == std::string::npos)
			dir = ".";
		else if(slashIndex == 0)
			dir = "/";
		else
			dir = fileName.substr(0, slashIndex);
		
		for(uint32_t i = 0; i < scene->mNumMaterials; i++)
		{
			const aiMaterial* aiMaterial = scene->mMaterials[i];
			m_Materials[i] = Material::CreateMaterial(scene->mMaterials[i]->GetName().data);

			if(aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0 )
			{
				aiString pathBuffer;
				if(aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &pathBuffer, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					std::string texturePath(pathBuffer.data);

					if(texturePath.substr(0, 2) == ".\\")
						texturePath = texturePath.substr(2, texturePath.size() - 2);
					std::string fullPath = dir + "/" + texturePath;

					// Check if Texture is Embedded
					const aiTexture* pAiEmbeddedTexture = m_aiScene->GetEmbeddedTexture(pathBuffer.C_Str());
					if(pAiEmbeddedTexture) {
						m_Materials[i]->DiffuseTexture = Texture2D::CreateTexture2D(fullPath, pAiEmbeddedTexture->pcData, pAiEmbeddedTexture->mWidth);
					} else {
						m_Materials[i]->DiffuseTexture = Texture2D::CreateTexture2D(fullPath);
					}

					if (!m_Materials[i]->DiffuseTexture) {
						success = false;
					}
				}
			}

			if(aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0 )
			{
				aiString pathBuffer;
				if(aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &pathBuffer, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					std::string texturePath(pathBuffer.data);

					if(texturePath.substr(0, 2) == ".\\")
						texturePath = texturePath.substr(2, texturePath.size() - 2);
					std::string fullPath = dir + "/" + texturePath;

					const aiTexture* pAiEmbeddedTexture = m_aiScene->GetEmbeddedTexture(pathBuffer.C_Str());

					if (pAiEmbeddedTexture) {
						m_Materials[i]->SpecularTexture = Texture2D::CreateTexture2D(fullPath, pAiEmbeddedTexture->pcData, pAiEmbeddedTexture->mWidth);
					} else {
						m_Materials[i]->SpecularTexture = Texture2D::CreateTexture2D(fullPath);
					}

					if (!m_Materials[i]->SpecularTexture)
						success = false;
				}
			}

			// Ambient
			aiColor3D AmbientColor(0.0);
			if(aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == AI_SUCCESS)
				m_Materials[i]->AmbientColor = {AmbientColor.r, AmbientColor.g, AmbientColor.b};

			// Diffuse 
			aiColor3D DiffuseColor(0.0);
			if(aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS)
				m_Materials[i]->DiffuseColor = {DiffuseColor.r, DiffuseColor.g, DiffuseColor.b};

			// TODO: Specular
			aiColor3D SpecularColor(0.0f);
			if(aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == AI_SUCCESS)
				m_Materials[i]->SpecularColor= {SpecularColor.r, SpecularColor.g, SpecularColor.b};

			// TODO: Specular Intensity (Shininess)
		}
		
		return success;
	}
	
	void Model::ProcessMesh(const aiMesh* pMesh)
	{
		// Populate Local Buffers with Vertex Data
		for(uint32_t i = 0; i < pMesh->mNumVertices; i++)
		{
			const aiVector3D& position = pMesh->mVertices[i];
			const aiVector3D& normal = pMesh->mNormals[i];
			const aiVector3D& texCoord = pMesh->HasTextureCoords(0) ? pMesh->mTextureCoords[0][i] : aiVector3D(0.0f);

			m_Positions.push_back({position.x, position.y, position.z});
			m_Normals.push_back({normal.x, normal.y, normal.z});
			m_TexCoords.push_back({texCoord.x, texCoord.y});
		}
		
		// Populate Local Index Buffer with Index Data
		for(uint32_t i = 0; i < pMesh->mNumFaces; i++)
		{
			const aiFace& face = pMesh->mFaces[i];

			// Faces Were Specified to be Triangulated
			assert(face.mNumIndices == 3);

			m_Indices.push_back(face.mIndices[0]);
			m_Indices.push_back(face.mIndices[1]);
			m_Indices.push_back(face.mIndices[2]);
		}
	}

	void Model::PopulateBuffers()
	{
		// Fill Mesh Vertex Buffers
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POSITION_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), m_Positions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(POSITION_LOCATION);
		glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), m_Normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), m_TexCoords.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Index Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);

		// Generate Model Instanced Transform Matrix Buffer
		glm::mat4* modelBuffer = new glm::mat4[m_MaxInstances];
		for(uint32_t i = 0; i < m_MaxInstances; i++)
			modelBuffer[i] = glm::mat4{1.0f};

		glGenBuffers(1, &m_ModelTransformMatrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_ModelTransformMatrixBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_MaxInstances, modelBuffer, GL_DYNAMIC_DRAW);

		// Make Transform Matrix Buffer Attrib Update Per Instance glVertexAttribDivisor(AttribLocation, 1)
		for(uint32_t i = 0; i < 4; i++)
		{
			glEnableVertexAttribArray(TRANSFORM_MATRIX_LOCATION + i);
			glVertexAttribPointer(TRANSFORM_MATRIX_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)* i)); 
			glVertexAttribDivisor(TRANSFORM_MATRIX_LOCATION + i, 1);
		}
		
		delete[] modelBuffer;
	}

	void Model::Clear()
	{
		// Clear Local Buffers
		m_Positions.clear();
		m_Normals.clear();
		m_Indices.clear();
		m_TexCoords.clear();

		// TODO: Clear GPU Buffers
	}
}
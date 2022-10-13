#include "Model.h"

#include <glad/glad.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define TEX_COORD_LOCATION 2
#define TRANSFORM_MATRIX_LOCATION 3

namespace SGE {
	Model::Model(const std::string& modelPath, bool flipUVS)
		:m_RendererID(0)
	{
		m_Buffers.resize(BUFFER_TYPE::NUM_BUFFERS);

		// Generate Model VAO
		glGenVertexArrays(1, &m_RendererID);

		// Generate Mesh Buffers
		for(uint32_t i = 0; i< m_Buffers.size(); i++)
			glGenBuffers(1, &m_Buffers[i]);

		LoadModel(modelPath, flipUVS);
	}

	Model::~Model()
	{
		for(auto buffer : m_Buffers)
			glDeleteBuffers(1, &buffer);
		
		glDeleteBuffers(1, &m_ModelTransformMatrixBuffer);
		glDeleteVertexArrays(1, &m_RendererID);
	}
	
	Ref<Model> Model::CreateModel(const std::string& modelPath, bool flipUVS)
	{
		return CreateRef<Model>(modelPath, flipUVS);
	}
	
	void Model::AddInstance(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		// TODO: ROTATE AROUND ANY AXIS
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
		model = glm::scale(model, scale);

		glBindBuffer(GL_ARRAY_BUFFER, m_ModelTransformMatrixBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, m_NumInstances * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(model));
		m_NumInstances++;
	}

	void Model::LoadModel(const std::string& fileName, bool flipUVS)
	{
		glBindVertexArray(m_RendererID);
		// TODO: Add clear function to reuse 
		bool success = false;

		Assimp::Importer importer;

		uint32_t ASSIMP_IMPORT_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals | 
									   aiProcess_JoinIdenticalVertices;
		ASSIMP_IMPORT_FLAGS |= flipUVS ? aiProcess_FlipUVs : 0;

		const aiScene* scene = importer.ReadFile(fileName.c_str(), ASSIMP_IMPORT_FLAGS);

		if (!scene)
			std::cout << "Failed to load model at " << fileName << "\n";

		success = ParseScene(scene, fileName);
		success = ProcessMaterials(scene, fileName);
		if (!success)
			std::cout << "Failed to parse model " << fileName << "\n";

		
		// Clear local buffers
		 m_Positions.clear();
		 m_Normals.clear();
		 m_Indices.clear();
		 m_TexCoords.clear();

		glBindVertexArray(0);
	}
	
	void Model::Render(const Ref<Shader> shader)
	{
		glBindVertexArray(m_RendererID);

		for(uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			// bind material textures and properties
			uint32_t materialIndex = m_Meshes[i].m_MaterialIndex;
			assert(materialIndex < m_Materials.size());
			shader->SetVec3("u_Material.Ambient", m_Materials[materialIndex]->AmbientColor);
			shader->SetVec3("u_Material.Diffuse", m_Materials[materialIndex]->DiffuseColor);
			shader->SetVec3("u_Material.Specular", m_Materials[materialIndex]->SpecularColor);

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

			// Draw
			DrawMesh(m_Meshes[i]);

			// Unbind Properties
			shader->SetBool("u_Material.HasDiffuseTexture", false);
			shader->SetBool("u_Material.HasSpecularTexture", false);
		}

		m_NumInstances = 0;
	}

	void Model::DrawMesh(const Mesh& mesh)
	{
		// draw mesh
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh.NumIndices(), GL_UNSIGNED_INT, 
											(void*)(sizeof(uint32_t) * mesh.BaseIndex()),
											m_NumInstances, mesh.BaseVertex());
	}

	bool Model::ParseScene(const aiScene* scene, const std::string& fileName)
	{
		m_Meshes.resize(scene->mNumMeshes);
		m_Materials.resize(scene->mNumMaterials);

		// Assign mesh properties & count buffer sizes
		uint32_t nVertices = 0;
		uint32_t nIndices = 0;
		for(uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			m_Meshes[i].m_MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
			m_Meshes[i].m_NumIndices 	= scene->mMeshes[i]->mNumFaces * 3;
			m_Meshes[i].m_BaseVertex 	= nVertices;
			m_Meshes[i].m_BaseIndex  	= nIndices;

			nVertices += scene->mMeshes[i]->mNumVertices;
			nIndices  += m_Meshes[i].m_NumIndices;
		}

		// reserve buffers
		m_Positions.reserve(nVertices);
		m_Normals.reserve(nVertices);
		m_TexCoords.reserve(nVertices);

		m_Indices.reserve(nIndices);

		// Process each mesh
		for(uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			const aiMesh* aiMesh = scene->mMeshes[i];
			ProcessMesh(aiMesh);
		}
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
					m_Materials[i]->DiffuseTexture = Texture2D::CreateTexture2D(fullPath);

					if (!m_Materials[i]->DiffuseTexture)
						success = false;
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
					m_Materials[i]->SpecularTexture = Texture2D::CreateTexture2D(fullPath);

					if (!m_Materials[i]->SpecularTexture)
						success = false;
				}
			}

			// Specular Intesntiy (Shininess)

			// AMBIENT 
			aiColor3D AmbientColor(0.0);
			if(aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == AI_SUCCESS)
			{
				printf("Loaded Ambient Color: [%f, %f, %f]\n", AmbientColor.r, AmbientColor.g, AmbientColor.b);
				m_Materials[i]->AmbientColor = {AmbientColor.r, AmbientColor.g, AmbientColor.b};
			}

			// DIFFUSE 
			aiColor3D DiffuseColor(0.0);
			if(aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS)
			{
				printf("Loaded Diffuse Color: [%f, %f, %f]\n", DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
				m_Materials[i]->DiffuseColor = {DiffuseColor.r, DiffuseColor.g, DiffuseColor.b};
			}

			// TODO: SPECULAR
			aiColor3D SpecularColor(0.0f);
			if(aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == AI_SUCCESS)
			{
				printf("Loaded Specular Color: [%f, %f, %f]\n", SpecularColor.r, SpecularColor.g, SpecularColor.b);
				m_Materials[i]->SpecularColor= {SpecularColor.r, SpecularColor.g, SpecularColor.b};
			}
		}
		
		return success;
	}
	
	void Model::ProcessMesh(const aiMesh* aiMesh)
	{
		// populate local vertex buffer
		for(uint32_t i = 0; i < aiMesh->mNumVertices; i++)
		{
			const aiVector3D& position = aiMesh->mVertices[i];
			const aiVector3D& normal = aiMesh->mNormals[i];
			const aiVector3D& texCoord = aiMesh->HasTextureCoords(0) ? aiMesh->mTextureCoords[0][i] : aiVector3D(0.0f);

			m_Positions.push_back({position.x, position.y, position.z});
			m_Normals.push_back({normal.x, normal.y, normal.z});
			m_TexCoords.push_back({texCoord.x, texCoord.y});
		}
		
		// populate index buffer
		for(uint32_t i = 0; i < aiMesh->mNumFaces; i++)
		{
			const aiFace& face = aiMesh->mFaces[i];

			// faces were specified to be triangulated
			assert(face.mNumIndices == 3);

			m_Indices.push_back(face.mIndices[0]);
			m_Indices.push_back(face.mIndices[1]);
			m_Indices.push_back(face.mIndices[2]);
		}
	}
	
	void Model::PopulateBuffers()
	{
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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);

		// Generate model instanced buffer
		glm::mat4* modelBuffer = new glm::mat4[m_MaxInstances];
		for(uint32_t i = 0; i < m_MaxInstances; i++)
		{
			modelBuffer[i] = glm::mat4{1.0f};
		}
		glGenBuffers(1, &m_ModelTransformMatrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_ModelTransformMatrixBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_MaxInstances, modelBuffer, GL_DYNAMIC_DRAW);

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
		// TODO: 0 out buffers
	}
}
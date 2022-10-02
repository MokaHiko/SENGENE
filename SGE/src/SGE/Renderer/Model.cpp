#include "Model.h"

#include <glad/glad.h>

#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define TEX_COORD_LOCATION 2

namespace SGE {
	Model::Model(const std::string& modelPath)
		:m_RendererID(0)
	{
		m_Buffers.resize(BUFFER_TYPE::NUM_BUFFERS);

		for(uint32_t i = 0; i< m_Buffers.size(); i++)
			glGenBuffers(1, &m_Buffers[i]);
	
		glGenVertexArrays(1, &m_RendererID);

		LoadModel(modelPath);
	}

	Model::~Model()
	{
		for(auto buffer : m_Buffers)
			glDeleteBuffers(1, &buffer);
		
		glDeleteVertexArrays(1, &m_RendererID);
	}
	
	Ref<Model> Model::CreateModel(const std::string& modelPath)
	{
		return CreateRef<Model>(modelPath);
	}

	void Model::LoadModel(const std::string& fileName)
	{
		glBindVertexArray(m_RendererID);
		// TODO: Add clear function to reuse 
		bool success = false;

		Assimp::Importer importer;

		uint32_t ASSIMP_IMPORT_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;
		const aiScene* scene = importer.ReadFile(fileName.c_str(), ASSIMP_IMPORT_FLAGS);

		if (!scene)
			std::cout << "Failed to load model at " << fileName << "\n";

		success = ParseScene(scene, fileName);
		if (!success)
			std::cout << "Failed to parse model at " << fileName << "\n";
		
		// TODO: local buffers
		 m_Positions.clear();
		 m_Normals.clear();
		 m_Indices.clear();
		 m_TexCoords.clear();

		glBindVertexArray(0);
	}
	
	void Model::Render()
	{
		glBindVertexArray(m_RendererID);

		for(uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			// bind material
			uint32_t materialIndex = m_Meshes[i].m_MaterialIndex;
			assert(materialIndex < m_Textures.size());
			if (m_Textures[materialIndex] != nullptr)
				m_Textures[materialIndex]->Bind();
			
			// draw mesh
			glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].m_NumIndices, GL_UNSIGNED_INT, 
									(void*)(sizeof(uint32_t) * m_Meshes[i].m_BaseIndex), 
									m_Meshes[i].m_BaseVertex);
		}

		glBindVertexArray(0);
	}

	bool Model::ParseScene(const aiScene* scene, const std::string& fileName)
	{
		m_Meshes.resize(scene->mNumMeshes);
		m_Textures.resize(scene->mNumMaterials);
		
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

		// Process Model Materials
		if (!ProcessMaterials(scene, fileName))
			std::cout << "Failed to load model materials\n";
		
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
			m_Textures[i] = nullptr;
			// process diffuse texture
			if(aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0 )
			{
				aiString pathBuffer;
				if(aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &pathBuffer, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					std::string texturePath(pathBuffer.data);

					if(texturePath.substr(0, 2) == ".\\")
						texturePath = texturePath.substr(2, texturePath.size() - 2);
					
					std::string fullPath = dir + "/" + texturePath;
					m_Textures[i] = Texture2D::CreateTexture2D(fullPath);

					if (!m_Textures[i])
						success = false;
				}
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
	}

	void Model::Clear()
	{
		// TODO: 0 out buffers
	}
}
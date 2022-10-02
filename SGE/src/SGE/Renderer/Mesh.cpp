#include "Mesh.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace SGE {
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		:m_VertexCount(vertices.size()), m_Indices(indices)
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		uint32_t VBO, EBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

		// Vertex Attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 0);
		glVertexAttribDivisor(2, 0);

		// Model Matrix Attribute
		glm::mat4* modelMatrices = new glm::mat4[MAX_INSTANCES]; 
		for(uint32_t i = 0; i <  MAX_INSTANCES; i++)
		{
			glm::mat4 modelMatrix(1.0f);
			modelMatrices[i] = modelMatrix;
		}

		glGenBuffers(1, &m_PositionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_PositionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * MAX_INSTANCES, modelMatrices, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 1));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 2));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 3));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &VBO);

		delete[] modelMatrices;
	}

	Mesh::~Mesh()
	{
		glDeleteBuffers(1, &m_PositionBuffer);
		glDeleteBuffers(1, &m_TextureUnitBuffer);

		glDeleteVertexArrays(1, &m_VAO);
	}
	
	Ref<Mesh> Mesh::CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		return CreateRef<Mesh>(vertices, indices);
	}

	void Mesh::AddInstance(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) *
								glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0, 0.0, 0.0)) *
								glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0, 1.0, 0.0)) *
								glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0, 0.0, 1.0)) *
							    glm::scale(glm::mat4(1.0f), scale);
		// TODO: possibly only update instances when change occured
		// TODO: upload top 4 by 3 matrix instead of 4 x 4 for performance

		glBindBuffer(GL_ARRAY_BUFFER, m_PositionBuffer);
		glBufferSubData(GL_ARRAY_BUFFER,  m_InstanceCount * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(modelMatrix));

		m_InstanceCount++;
	}
	
	void Mesh::DrawInstanced()
	{
		glBindVertexArray(m_VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, m_VertexCount, m_InstanceCount);
		m_InstanceCount = 0;
	}
	
	void Mesh::ProcessMesh()
	{
	}
}
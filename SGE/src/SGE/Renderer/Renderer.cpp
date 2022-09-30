#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SGE{
    std::unordered_map<Ref<Material>, Renderer::MaterialGroup> Renderer::m_MaterialGroups; // TODO: change to a type of weak ptr  
	SceneData Renderer::m_SceneData{};
    uint32_t Renderer::m_MeshCount = 0;

	Renderer::Renderer(){}
	void Renderer::Init()
	{
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	
    void Renderer::Configure(SceneData& sceneData)
	{
		m_SceneData = sceneData;

		// Configure Model Materials
		m_SceneData.SceneShader->Bind();
		m_SceneData.SceneShader->SetInt("u_Material.texture_diffuse1", 0);
		m_SceneData.SceneShader->SetInt("u_Material.texture_specular1", 1);

		// directional lights
		m_SceneData.SceneShader->SetVec3("u_DirLight.Direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		m_SceneData.SceneShader->SetVec3("u_DirLight.Ambient", glm::vec3(0.05f));
		m_SceneData.SceneShader->SetVec3("u_DirLight.Diffuse", glm::vec3(0.4f));
		m_SceneData.SceneShader->SetVec3("u_DirLight.Specular", glm::vec3(0.5f));
	}

	void Renderer::Begin()
	{
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)m_SceneData.SceneWidth / (float)m_SceneData.SceneHeight, 0.1f, 1000.0f);
		auto& camera = m_SceneData.MainCamera.GetComponent<Camera3DComponent>();
		auto& cameraPosition = m_SceneData.MainCamera.GetComponent<TransformComponent>();

		// Configure Cameras
		m_SceneData.SceneShader->Bind();
		m_SceneData.SceneShader->SetMat4("projection", projectionMatrix);
		m_SceneData.SceneShader->SetMat4("view", camera.camera.GetViewMatrix());

		m_SceneData.SceneShader->SetVec3("u_MainCameraPos", cameraPosition.Position);
		
		// Point Lights
		int ctr = 0;
		for(Entity pl: m_SceneData.PointLights)
		{
			PointLightComponent& pointLight = pl.GetComponent<PointLightComponent>();
			TransformComponent& pointLightPosition = pl.GetComponent<TransformComponent>();

			std::string index = std::to_string(ctr);
			m_SceneData.SceneShader->SetVec3(std::string("u_PointLights["  + index + "].Position"),  cameraPosition.Position);
			m_SceneData.SceneShader->SetVec3(std::string("u_PointLights["  + index + "].Ambient"),   pointLight.Ambient);
			m_SceneData.SceneShader->SetVec3(std::string("u_PointLights["  + index + "].Diffuse"),   pointLight.Diffuse);
			m_SceneData.SceneShader->SetVec3(std::string("u_PointLights["  + index + "].Specular"),  pointLight.Specular);

			m_SceneData.SceneShader->SetFloat(std::string("u_PointLights[" + index + "].Constant"),  pointLight.Constant);
			m_SceneData.SceneShader->SetFloat(std::string("u_PointLights[" + index + "].Linear"),    pointLight.Linear);
			m_SceneData.SceneShader->SetFloat(std::string("u_PointLights[" + index + "].Quadratic"), pointLight.Quadratic);
			ctr++;
		}
		
		m_SceneData.SceneShader->SetInt("u_NPointLights", m_SceneData.PointLights.size());
	}

	void Renderer::End()
	{
		for (auto& it : m_MaterialGroups)
		{
			// bind material values and textures
			glActiveTexture(GL_TEXTURE0); 
			glBindTexture(GL_TEXTURE_2D, it.first->DiffuseTextureID);
			glActiveTexture(GL_TEXTURE1); 
			glBindTexture(GL_TEXTURE_2D, it.first->SpecularTextureID);

			m_SceneData.SceneShader->SetFloat("u_Material.shininess", it.first->Shininess);

			// draw mesh instances
			for(Ref<Mesh> mesh : it.second.m_Meshes)
				mesh->Draw();
			// TODO:: HANDLE MESH WITH DIFFERENT MATERIALS
		}
	}
	
	void Renderer::Draw(Ref<Mesh> mesh, Ref<Material> material, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		// create material group if renderer has yet to see it
		if(m_MaterialGroups.find(material) == m_MaterialGroups.end())
			m_MaterialGroups[material] = MaterialGroup(mesh);
		else
			m_MaterialGroups[material].m_Meshes.insert(mesh);

		mesh->AddInstance(position, rotation, scale); 
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	Renderer::~Renderer(){}
}
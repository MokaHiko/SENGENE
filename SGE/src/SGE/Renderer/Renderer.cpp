#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SGE{
    std::unordered_set <Ref<Model>> Renderer::m_Models;
	SceneData Renderer::m_SceneData{};

	Renderer::Renderer(){}
	void Renderer::Init()
	{
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	
    void Renderer::Configure(SceneData& sceneData)
	{
		m_SceneData = sceneData;

		// Configure Model Materials
		m_SceneData.SceneShader->Bind();
		m_SceneData.SceneShader->SetInt("u_Material.texture_diffuse1", 0);
		m_SceneData.SceneShader->SetInt("u_Material.texture_specular1", 1);

		// directional lights
		if (sceneData.DirectionalLight)
		{
			auto& dirLight = sceneData.DirectionalLight.GetComponent<DirectionalLightComponent>();
			auto& dirLightTransform = sceneData.DirectionalLight.GetComponent<TransformComponent>();
			m_SceneData.SceneShader->SetVec3("u_DirLight.Direction", -dirLightTransform.Position);
			m_SceneData.SceneShader->SetVec3("u_DirLight.Ambient", dirLight.Ambient);
			m_SceneData.SceneShader->SetVec3("u_DirLight.Diffuse", dirLight.Diffuse);
			m_SceneData.SceneShader->SetVec3("u_DirLight.Specular", dirLight.Specular);
		}
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

		// Directional Light
		m_SceneData.SceneShader->SetVec3("u_DirLight.Direction", -m_SceneData.DirectionalLight.GetComponent<TransformComponent>().Position);
		
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
		for (auto& model : m_Models)
			model->Render(m_SceneData.SceneShader);

		m_Models.clear();
	}
	
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}
	
	void Renderer::Draw(Ref<Model> model, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		m_Models.insert(model);
		model->AddInstance(position, rotation, scale);
	}

	Renderer::~Renderer(){}
}
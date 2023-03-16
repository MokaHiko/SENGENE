#include "SkinnedMeshRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SGE
{
	std::unordered_set<Ref<AnimatedModel>> SkinnedMeshRenderer::m_Models;
	SceneData SkinnedMeshRenderer::m_SceneData{};
	Ref<Shader> SkinnedMeshRenderer::m_Shader = nullptr;

	SkinnedMeshRenderer::SkinnedMeshRenderer() {}
	void SkinnedMeshRenderer::Init()
	{
		// Load Renderer's Default Resources
		m_Shader = Shader::GetShader("assets/shaders/phong_instanced_shader_animated");
	}

	void SkinnedMeshRenderer::Configure(SceneData &sceneData)
	{
		m_SceneData = sceneData;

		// Configure Model Materials
		m_Shader->Bind();
		m_Shader->SetInt("u_Material.texture_diffuse1", 0);
		m_Shader->SetInt("u_Material.texture_specular1", 1);

		// directional lights
		if (sceneData.DirectionalLight)
		{
			auto &dirLight = sceneData.DirectionalLight.GetComponent<DirectionalLightComponent>();
			auto &dirLightTransform = sceneData.DirectionalLight.GetComponent<TransformComponent>();
			m_Shader->SetVec3("u_DirLight.Direction", -dirLightTransform.Position);
			m_Shader->SetVec3("u_DirLight.Ambient", dirLight.Ambient);
			m_Shader->SetVec3("u_DirLight.Diffuse", dirLight.Diffuse);
			m_Shader->SetVec3("u_DirLight.Specular", dirLight.Specular);
		}
	}

	void SkinnedMeshRenderer::Begin()
	{
		// Configure Renderer Settings
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		// Bind Camera Properties
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)m_SceneData.SceneWidth / (float)m_SceneData.SceneHeight, 0.1f, 1000.0f);
		auto &camera = m_SceneData.MainCamera.GetComponent<Camera3DComponent>();
		auto &cameraPosition = m_SceneData.MainCamera.GetComponent<TransformComponent>();

		m_Shader->Bind();
		m_Shader->SetMat4("projection", projectionMatrix);
		m_Shader->SetMat4("view", camera.camera.GetViewMatrix());
		m_Shader->SetVec3("u_MainCameraPos", cameraPosition.Position);

		// Bind Directional Light Properties
		m_Shader->SetVec3("u_DirLight.Direction", -m_SceneData.DirectionalLight.GetComponent<TransformComponent>().Position);

		// Bind Point Lights Properties
		int ctr = 0;
		m_Shader->SetInt("u_NPointLights", m_SceneData.PointLights.size());
		for (Entity pl : m_SceneData.PointLights)
		{
			PointLightComponent &pointLight = pl.GetComponent<PointLightComponent>();
			TransformComponent &pointLightPosition = pl.GetComponent<TransformComponent>();

			std::string index = std::to_string(ctr);
			m_Shader->SetVec3(std::string("u_PointLights[" + index + "].Position"), cameraPosition.Position);
			m_Shader->SetVec3(std::string("u_PointLights[" + index + "].Ambient"), pointLight.Ambient);
			m_Shader->SetVec3(std::string("u_PointLights[" + index + "].Diffuse"), pointLight.Diffuse);
			m_Shader->SetVec3(std::string("u_PointLights[" + index + "].Specular"), pointLight.Specular);

			m_Shader->SetFloat(std::string("u_PointLights[" + index + "].Constant"), pointLight.Constant);
			m_Shader->SetFloat(std::string("u_PointLights[" + index + "].Linear"), pointLight.Linear);
			m_Shader->SetFloat(std::string("u_PointLights[" + index + "].Quadratic"), pointLight.Quadratic);
			ctr++;
		}

		// Gizmos
		m_Shader->SetInt("u_FocusedBoneIndex", m_SceneData.FocusedBoneIndex);
	}

	void SkinnedMeshRenderer::End()
	{
		for (auto &model : m_Models)
			model->Render(m_Shader);

		m_Models.clear();
	}

	void SkinnedMeshRenderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	void SkinnedMeshRenderer::Draw(Ref<AnimatedModel> model, const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
	{
		m_Models.insert(model);
		model->AddInstance(position, rotation, scale);
	}

	SkinnedMeshRenderer::~SkinnedMeshRenderer() {}
}
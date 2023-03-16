#include "GrassRenderer.h"
#include <GLFW/glfw3.h>

namespace SGE
{
	Ref<Model> GrassRenderer::m_GrassModel;
	SceneData GrassRenderer::m_SceneData{};
	Ref<Shader> GrassRenderer::m_Shader = nullptr;

	GrassRenderer::GrassRenderer()
	{
	}

	GrassRenderer::~GrassRenderer()
	{
	}

	void GrassRenderer::Init(Ref<Model> grassModel, Ref<Shader> grassShader)
	{
		// Assign Grass Default Grass Shader
		if (grassShader == nullptr)
			m_Shader = Shader::CreateShader("assets/shaders/phong_instanced_shader.vert", "assets/shaders/phong_instanced_shader.frag");
		else
			m_Shader = grassShader;

		// Assign Grass Model
		m_GrassModel = grassModel;
	}

	void GrassRenderer::Configure(SceneData &sceneData)
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

		m_Shader->SetFloat("u_Time", 0);
	}

	void GrassRenderer::Begin()
	{
		// Renderer Settings
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		// Bind Camera Properties
		auto &camera = m_SceneData.MainCamera.GetComponent<Camera3DComponent>();
		auto &cameraPosition = m_SceneData.MainCamera.GetComponent<TransformComponent>();

		m_Shader->Bind();
		m_Shader->SetMat4("projection", m_SceneData.ProjectionMatrix);
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

		// Grass Specific
		m_Shader->SetFloat("u_Time", (float)glfwGetTime());
	}

	void GrassRenderer::End()
	{
		// Render Grass Models and do not clear instances
		m_GrassModel->Render(m_Shader, false);
	}

	void GrassRenderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	void GrassRenderer::AddInstance(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
	{
		m_GrassModel->AddInstance(position, rotation, scale);
	}
}
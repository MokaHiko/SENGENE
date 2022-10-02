#include "FeatureTest3D.h"

#include "FeatureTest3D.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Scripts/CameraController.h"
#include "Scripts/Unit.h"

#include <imgui/imgui.h>

FeatureTest3D::~FeatureTest3D()
{

}

void FeatureTest3D::OnAttach()
{
	// Scene Configuration
	m_Scene = SGE::CreateRef<SGE::Scene>();
	
	// Spawn Entities
	{
		SGE::Entity camera = m_Scene->CreateEntity("Main Camera");
		camera.AddComponent<SGE::Camera3DComponent>(true);
		camera.AddComponent<SGE::NativeScriptComponent>().Bind<CameraController>();
		camera.AddComponent<SGE::PointLightComponent>();
		m_SceneData.PointLights.push_back(camera); // add point light

		// attach camera as Maincamera to scene data
		m_SceneData.MainCamera = camera;
		m_SceneData.SceneShader = SGE::Shader::CreateShader("./assets/shaders/deffered_shader.vert", "./assets/shaders/deffered_shader.frag");
	}

	m_Model = SGE::Model::CreateModel("assets/models/spiderbot/source/spiderbot2.gltf");
	
	SGE::Renderer::Configure(m_SceneData);
}

void FeatureTest3D::OnDetach()
{
}

void FeatureTest3D::OnEvent(SGE::Event& event)
{
	if(event.GetEventType() == SGE::EventType::MouseMove) 
	{
		auto view = m_Scene->Registry().view<SGE::EventWatcherComponent<SGE::MouseMoveEvent>>();
		for(auto entity : view)
		{
			auto& eventWatcher = view.get<SGE::EventWatcherComponent<SGE::MouseMoveEvent>>(entity);
			eventWatcher.EntityCallBack(*(SGE::MouseMoveEvent*)&event);
		}
	}
}

void FeatureTest3D::OnUpdate(SGE::TimeStep ts)
{
	// Render Setup
	SGE::Renderer::Begin();

	// Update Scene ECS
	m_Scene->Update(ts);

	// Actual Rendering
	SGE::Renderer::End();
	
	m_SceneData.SceneShader->Bind();
	glm::mat4 transformMatrix = glm::mat4(1.0f);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(-10, 1.0, -10.0));
	transformMatrix = glm::rotate(transformMatrix, glm::sin((float)glfwGetTime()), glm::vec3(0, 1.0, 0.0));
	transformMatrix = glm::scale(transformMatrix, glm::vec3(1));
	m_SceneData.SceneShader->SetMat4("model", transformMatrix);
	m_Model->Render();

	transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(10, 1.0, -10.0));
	transformMatrix = glm::rotate(transformMatrix, glm::sin((float)glfwGetTime()), glm::vec3(0, 1.0, 0.0));
	transformMatrix = glm::scale(transformMatrix, glm::vec3(1));
	m_SceneData.SceneShader->SetMat4("model", transformMatrix);

	m_Model->Render();
}

bool show = true;
void FeatureTest3D::OnImGuiRender()
{
	ImGui::ShowDemoWindow(&show);
}

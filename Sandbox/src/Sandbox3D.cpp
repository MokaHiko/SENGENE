#include "Sandbox3D.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Scripts/CameraController.h"
#include "Scripts/Unit.h"

#include <imgui/imgui.h>

Sandbox3D::~Sandbox3D()
{
}

void Sandbox3D::OnAttach()
{
	// Scene Configuration
	m_Scene = SGE::CreateRef<SGE::Scene>();
	
	// Model Loading
	testImage  = SGE::Texture2D::CreateTexture2D("./assets/textures/container2.png");
	testImage1 = SGE::Texture2D::CreateTexture2D("./assets/textures/dirt.jpg");
	testImage2 = SGE::Texture2D::CreateTexture2D("./assets/textures/container2_specular.png");
	SGE::Ref<SGE::Material> crateMaterial = SGE::Material::CreateMaterial(testImage, testImage2);
	SGE::Ref<SGE::Material> dirtMaterial = SGE::Material::CreateMaterial(testImage1);

	float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	std::vector<SGE::Vertex> meshVertices{};
	for(int i = 0; i < 6 * 6 * 8; i += 8)
	{
		SGE::Vertex vertex{};
		
		int offset = 0;
		vertex.Position.x = vertices[i + offset++];  
		vertex.Position.y = vertices[i + offset++];  
		vertex.Position.z = vertices[i + offset++];  

		vertex.Normal.x = vertices[i + offset++];  
		vertex.Normal.y = vertices[i + offset++];  
		vertex.Normal.z = vertices[i + offset++];  

		vertex.TexCoord.x = vertices[i + offset++];
		vertex.TexCoord.y = vertices[i + offset];

		meshVertices.emplace_back(vertex);
	}
	
	// Spawn Entities
	{
		SGE::Entity camera = m_Scene->CreateEntity("Main Camera");
		camera.AddComponent<SGE::Camera3DComponent>(true);
		camera.AddComponent<SGE::NativeScriptComponent>().Bind<CameraController>();
		camera.AddComponent<SGE::PointLightComponent>();
		m_SceneData.PointLights.push_back(camera); // add point light

		// attach camera as Maincamera to scene data
		m_SceneData.MainCamera = camera;
		m_SceneData.SceneShader = SGE::Shader::CreateShader("./assets/shaders/basic_shader.vert", "./assets/shaders/basic_shader.frag");
	}

	auto mesh = SGE::Mesh::CreateMesh(meshVertices);
	auto dirtmesh = SGE::Mesh::CreateMesh(meshVertices);

	{
		SGE::Entity dirt = m_Scene->CreateEntity("dirt", {1.5f, 0, 1.5});
		auto& meshComponent = dirt.AddComponent<SGE::MeshComponent>(dirtmesh, dirtMaterial);
	}

	int root = 10;
	for(int i = -root ; i < root / 2; i++)
	{
		for (int j = -root ; j < root / 2; j++)
		{
			SGE::Entity cube = m_Scene->CreateEntity("crate", { i * 1.5f, j * 1.5f, -10.0f });
			auto& meshComponent = cube.AddComponent<SGE::MeshComponent>(mesh, crateMaterial);
			cube.AddComponent<SGE::NativeScriptComponent>().Bind<Unit>();
		}
	}

	SGE::Renderer::Configure(m_SceneData);
}

void Sandbox3D::OnDetach()
{
}

void Sandbox3D::OnEvent(SGE::Event& event)
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

void Sandbox3D::OnUpdate(SGE::TimeStep ts)
{
	// Render Setup
	SGE::Renderer::Begin();

	// Update Scene ECS
	m_Scene->Update(ts);

	// Actual Rendering
	SGE::Renderer::End();
}

bool show = true;
void Sandbox3D::OnImGuiRender()
{
	ImGui::ShowDemoWindow(&show);
}
#include "Sandbox2D.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Scripts/CameraController.h"
#include "Scripts/Unit.h"

#include <Core/Application.h>
#include <imgui/imgui.h>

Sandbox2D::~Sandbox2D()
{
}

void Sandbox2D::OnAttach()
{
	static const float z = -30;

	// Scene Configuration
	m_Scene = SGE::CreateRef<SGE::Scene>();
	SGE::FramebufferSpecification spec;
	spec.Width = 1280;
	spec.Height = 720;
	m_Framebuffer = SGE::Framebuffer::CreateFramebuffer(spec);
	m_ViewPortSize = { spec.Width, spec.Height };
	m_SampleTexture = SGE::Texture2D::CreateTexture2D("assets/textures/dirt.jpg");
	
	// Configure UI Panels
	{
		m_SceneHierarchyPanel.SetContext(m_Scene);
	}

	// Spawn Entities
	{
		// cameras
		SGE::Entity camera = m_Scene->CreateEntity("Main Camera", {0, 10, 0});
		camera.AddComponent<SGE::Camera3DComponent>(true);
		camera.AddComponent<SGE::NativeScriptComponent>().Bind<CameraController>();
		camera.AddComponent<SGE::PointLightComponent>();
		
		// lights
		SGE::Entity dirLight = m_Scene->CreateEntity("Directional Light", { 0.0, 100.0, 90});
		dirLight.AddComponent<SGE::DirectionalLightComponent>();

		// units
		// TODO LOWER: MODEL SIZE
		auto cubeModel = SGE::Model::CreateModel("assets/models/cube/cube.obj");
		auto redCubeModel = SGE::Model::CreateModel("assets/models/red_cube/redcube.obj");
		auto treeModel = SGE::Model::CreateModel("assets/models/tree/Lowpoly_tree_sample.obj");
		auto barrelModel = SGE::Model::CreateModel("assets/models/barrel/wine_barrel_01_4k.obj", true);

		{
			SGE::Entity terrain = m_Scene->CreateEntity("Terrain", {0, 0, z - 5});
			float sideLength = 1000;
			terrain.GetComponent<SGE::TransformComponent>().Scale = {sideLength, 5, 1};
			terrain.AddComponent<SGE::MeshRendererComponent>(cubeModel);
			terrain.AddComponent<SGE::RigidBody2DComponent>();
			terrain.AddComponent<SGE::BoxCollider2DComponent>();
		}

		{
			std::string name = "tree";
			SGE::Entity  e= m_Scene->CreateEntity(name, {-50, 10, z});
			e.AddComponent<SGE::MeshRendererComponent>(treeModel);
			auto& rb2d = e.AddComponent<SGE::RigidBody2DComponent>();
			rb2d.Type = SGE::RigidBody2DComponent::BodyType::Dynamic;
			e.AddComponent<SGE::BoxCollider2DComponent>();
		}

		// Set Up Scene
		m_SceneData.SceneShader = SGE::Shader::CreateShader("./assets/shaders/deffered_shader.vert", "./assets/shaders/deffered_shader.frag");
		m_SceneData.MainCamera = camera;

		m_SceneData.DirectionalLight = dirLight;
		m_SceneData.PointLights.push_back(camera);

		SGE::Renderer::Configure(m_SceneData);
	}
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnEvent(SGE::Event& event)
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
	if(event.GetEventType() == SGE::EventType::WindowResize) 
	{
		SGE::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<SGE::WindowResizeEvent>(std::bind(&Sandbox2D::OnWindowResize, this, std::placeholders::_1));
	}
}

void Sandbox2D::OnUpdate(SGE::TimeStep ts)
{
	// Render Setup
	m_Framebuffer->Bind();
	SGE::Renderer::Begin();

	// Update Scene ECS 	
	m_Scene->Update(ts);

	// Actual Rendering
	SGE::Renderer::End();
	m_Framebuffer->Unbind();
}

void Sandbox2D::OnImGuiRender()
{
	static bool opt_isOpen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &opt_isOpen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
		std::cout << "Docking Disabled" << std::endl;
    }

	// Menus
	if (ImGui::BeginMainMenuBar())
	{
		// Application Menus
		if (ImGui::BeginMenu("File"))
		{
			ShowFileMenuHierarchy();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Flag: NoSplit",                "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
				if (ImGui::MenuItem("Flag: NoResize",               "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
				if (ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
				if (ImGui::MenuItem("Flag: PassthruCentralNode",    "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
				ImGui::Separator();

				if (ImGui::MenuItem("Close", NULL, false, &opt_isOpen != NULL))
					opt_isOpen = false;
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		
		// Panel Menus
		m_SceneHierarchyPanel.ShowMenu();	

		ImGui::EndMainMenuBar();
	}

	// Panels
	m_SceneHierarchyPanel.OnImGuiRender();
    ImGui::End();

	// Application Viewport of Dockspace
	ImGui::Begin("View Port");
	static std::string runLabel = "Play";
	if (ImGui::Button(runLabel.c_str()))
	{
		if(m_Scene->m_SceneState == SGE::SCENE_STATE::PAUSE)
		{
			m_Scene->OnScenePlay(); 
			runLabel = "Pause";
		}
		else
		{
			m_Scene->OnSceneStop();
			runLabel = "Play";
		}
	}

	uint32_t textureID = m_Framebuffer->GetColorAttachment();
	ImGui::Image((void*)textureID, ImVec2(m_ViewPortSize.x, m_ViewPortSize.y), ImVec2(0,1), ImVec2(1,0));

	ImGui::End();
}

bool Sandbox2D::OnWindowResize(SGE::WindowResizeEvent& event)
{
	m_Framebuffer->Resize(event.GetWidth(), event.GetHeight());
	m_ViewPortSize = { event.GetWidth(), event.GetHeight() };
	return false;
}
void Sandbox2D::ShowFileMenuHierarchy()
{
	if (ImGui::MenuItem("New")) {}
	if (ImGui::MenuItem("Open", "Ctrl+O")) {}
	if (ImGui::BeginMenu("Open Recent"))
	{
		ImGui::MenuItem("fish_hat.c");
		ImGui::MenuItem("fish_hat.inl");
		ImGui::MenuItem("fish_hat.h");
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save", "Ctrl+S")) {}
	if (ImGui::MenuItem("Save As..")) {}

	ImGui::Separator();
	if (ImGui::BeginMenu("Options"))
	{
		static bool enabled = true;
		ImGui::MenuItem("Enabled", "", &enabled);
		ImGui::BeginChild("child", ImVec2(0, 60), true);
		for (int i = 0; i < 10; i++)
			ImGui::Text("Scrolling Text %d", i);
		ImGui::EndChild();
		static float f = 0.5f;
		static int n = 0;
		ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
		ImGui::InputFloat("Input", &f, 0.1f);
		ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Colors"))
	{
		float sz = ImGui::GetTextLineHeight();
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
			ImGui::Dummy(ImVec2(sz, sz));
			ImGui::SameLine();
			ImGui::MenuItem(name);
		}
		ImGui::EndMenu();
	}
 
	// Here we demonstrate appending again to the "Options" menu (which we already created above)
	// Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
	// In a real code-base using it would make senses to use this feature from very different code locations.
	if (ImGui::BeginMenu("Options")) // <-- Append!
	{
		static bool b = true;
		ImGui::Checkbox("SomeOption", &b);
		ImGui::EndMenu();
	}	

	if (ImGui::BeginMenu("Disabled", false)) // Disabled
	{
		IM_ASSERT(0);
	}
	if (ImGui::MenuItem("Checked", NULL, true)) {}
	if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}
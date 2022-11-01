#include "SceneHierarchyPanel.h"
#include <glm/gtc/type_ptr.hpp>

#include "Renderer/ResourceManager.h"
namespace SGE {
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
		:m_SceneContext(scene) 
	{
		m_SelectedEntity = {};
	}

	SceneHierarchyPanel::~SceneHierarchyPanel() {}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (!m_SceneContext)
			return;
		
		ImGui::Begin("Scene Hierarchy");
		ImGui::SetNextItemOpen(true);
		if (ImGui::TreeNode("Scene"))
		{
			ScenePopupWindows();

			// Scene Hierarchy
			if (m_align_label_with_current_x_position)
				ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

			auto view = m_SceneContext->Registry().view<TagComponent>();
			for(auto e : view)
				DrawEntityNode(Entity{e, m_SceneContext.get()});

			if (m_align_label_with_current_x_position)
				ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

			ImGui::TreePop();
		}
		ImGui::End();

		if(m_SelectedEntity)
			ShowSelectedComponents();
	}
	
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_SceneContext = scene;
		m_SelectedEntity = {};
	}
	
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
		// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
		ImGuiTreeNodeFlags node_flags = m_TreeNodeFlags;
		auto& tag = entity.GetComponent<TagComponent>();
		const bool is_selected = (entity == m_SelectedEntity);

		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx(tag.Tag.c_str(), node_flags, "%s", tag.Tag.c_str());
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			m_SelectedEntity = entity;
		if (m_test_drag_and_drop && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}
		if (node_open)
		{
			// TODO: Recursive Children
			ImGui::BulletText("Child 1\n Child 2");
			ImGui::TreePop();
		}
	}
	
	void SceneHierarchyPanel::ShowMenu()
	{
		if (ImGui::BeginMenu("Hierarchy Layout"))
		{
			ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow",       &m_TreeNodeFlags, ImGuiTreeNodeFlags_OpenOnArrow);
			ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", &m_TreeNodeFlags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
			ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth",    &m_TreeNodeFlags, ImGuiTreeNodeFlags_SpanAvailWidth);
			ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth",     &m_TreeNodeFlags, ImGuiTreeNodeFlags_SpanFullWidth);
			ImGui::Checkbox("Align label with current X position", &m_align_label_with_current_x_position);
			ImGui::Checkbox("Test tree node as drag source", &m_test_drag_and_drop);
			ImGui::EndMenu();
		}
	}
	
	void SceneHierarchyPanel::ShowSelectedComponents()
	{
		ImGui::Begin("Properties");
		if(m_SelectedEntity.HasComponent<TagComponent>())
		{
			auto& tag = m_SelectedEntity.GetComponent<TagComponent>();

			static char buffer[TagComponent::MAX_TAG_SIZE] = "";
			strcpy(buffer, tag.Tag.c_str());

			static bool isStatic = true;
			if (ImGui::InputText("##label", buffer, 128, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
				tag.Tag = buffer;

			ImGui::SameLine();
			ImGui::Checkbox("static", &isStatic); 

			ImGui::Separator();
		}

		if(m_SelectedEntity.HasComponent<TransformComponent>())
		{
			if (ImGui::CollapsingHeader("Transform Component"))
			{
				static float rate = 0.2f;
				auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();
				ImGui::DragFloat3("Position: ", glm::value_ptr(transform.Position), rate);
				ImGui::DragFloat3("Scale: ", glm::value_ptr(transform.Scale), rate);
				ImGui::DragFloat3("Rotation: ", glm::value_ptr(transform.Rotation), rate);
			}
		}

		if(m_SelectedEntity.HasComponent<MeshRendererComponent>())
		{ 
			if (ImGui::CollapsingHeader("Mesh Renderer"))
			{
				auto& model = m_SelectedEntity.GetComponent<MeshRendererComponent>().Model;

				ImGui::Text("Meshes : %d", model->GetNMeshes());
				ImGui::Text("Draw Calls: %d", model->GetNMaterials());	

				ImGui::Separator();
				ImGui::Text("Materials: %d", model->GetNMaterials());
				ImVec2 panelSize = ImGui::GetWindowContentRegionMax(); panelSize.x /= 2; panelSize.y /= 6;

				for(const auto& material : model->GetMaterials())
				{
					ImGui::Text("%s", material->Name.c_str());
					ImGui::ColorEdit3("Ambient", glm::value_ptr(material->AmbientColor), ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(material->DiffuseColor), ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::ColorEdit3("Specular", glm::value_ptr(material->SpecularColor), ImGuiColorEditFlags_NoInputs);

					if (material->DiffuseTexture)
					{
						ImGui::Text("Diffuse Texture");
						ImGui::Image((void*)material->DiffuseTexture->GetID(), panelSize, ImVec2(0,1), ImVec2(1,0));
					}
					if (material->SpecularTexture)
					{
						ImGui::SameLine();
						ImGui::Text("Specular Texture"); 
						ImGui::Image((void*)material->SpecularTexture->GetID(), panelSize, ImVec2(0,1), ImVec2(1,0));
					}
				}
			}
			
			if(m_SelectedEntity.HasComponent<RigidBody2DComponent>())
			{
				if (ImGui::CollapsingHeader("RigidBody Component"))
				{
					static float rate = 0.2f;
					auto& rb = m_SelectedEntity.GetComponent<RigidBody2DComponent>();
					switch (rb.Type)
					{
						case RigidBody2DComponent::BodyType::Static:
							ImGui::Text("Static");
							break;
						case RigidBody2DComponent::BodyType::Dynamic:
							ImGui::Text("Dynamic");
							break;
						case RigidBody2DComponent::BodyType::Kinematic:
							ImGui::Text("Kinematic");
							break;
						default:
							ImGui::Text("Uknown Body Type");
							break;
					};

					if (ImGui::RadioButton("Static", rb.Type == RigidBody2DComponent::BodyType::Static)) { rb.Type = RigidBody2DComponent::BodyType::Static; } ImGui::SameLine();
					if (ImGui::RadioButton("Dynamic", rb.Type == RigidBody2DComponent::BodyType::Dynamic)) { rb.Type = RigidBody2DComponent::BodyType::Dynamic; } ImGui::SameLine();
					if (ImGui::RadioButton("Kinematic", rb.Type == RigidBody2DComponent::BodyType::Kinematic)) { rb.Type = RigidBody2DComponent::BodyType::Kinematic; } ImGui::SameLine();
				}
			}
		}

		// Add Component PopUp
		{
			static const char* title = "+ Add New Component";
			if (ImGui::Button(title, ImVec2{ImGui::GetWindowWidth(), 0}) && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow) 
				&& ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow)) {ImGui::OpenPopup("add_component_pop_up");}

			if (ImGui::BeginPopup("add_component_pop_up"))
			{
				if (ImGui::BeginMenu("Renderable"))
				{
					if(ImGui::MenuItem("Cube"))
					{
						if(!m_SelectedEntity.HasComponent<MeshRendererComponent>())
							m_SelectedEntity.AddComponent<MeshRendererComponent>(ResourceManager::GetModel("assets/models/cube/cube.obj"));
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Physics"))
				{
					if(ImGui::MenuItem("RigidBody2D"))
					{
						if(!m_SelectedEntity.HasComponent<RigidBody2DComponent>())
							m_SelectedEntity.AddComponent<SGE::RigidBody2DComponent>();
					}

					if(ImGui::MenuItem("BoxCollider 2D"))
					{
						if(!m_SelectedEntity.HasComponent<BoxCollider2DComponent>())
							m_SelectedEntity.AddComponent<BoxCollider2DComponent>();
					}
					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

    void SceneHierarchyPanel::ScenePopupWindows()
	{
		 // Showing a menu with toggles
        if (ImGui::GetIO().MouseClicked[1] && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow) 
			&& ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow)) {ImGui::OpenPopup("my_toggle_popup");}

        if (ImGui::BeginPopup("my_toggle_popup"))
        {
            if (ImGui::BeginMenu("Add Entity"))
            {
                if(ImGui::MenuItem("Empty Entity"))
				{
					Entity e = m_SceneContext->CreateEntity();
				}
                if(ImGui::MenuItem("Cube Entity"))
				{
					Entity e = m_SceneContext->CreateEntity();
					e.AddComponent<MeshRendererComponent>(ResourceManager::GetModel("assets/models/cube/cube.obj"));
				}
                ImGui::EndMenu();
            }

            ImGui::Separator();
            ImGui::Text("Tooltip here");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("I am a tooltip over a popup");

            if (ImGui::Button("Stacked Popup"))
                ImGui::OpenPopup("another popup");
            if (ImGui::BeginPopup("another popup"))
            {
                if (ImGui::BeginMenu("Sub-menu"))
                {
                    ImGui::MenuItem("Click me");
                    if (ImGui::Button("Stacked Popup"))
                        ImGui::OpenPopup("another popup");
                    if (ImGui::BeginPopup("another popup"))
                    {
                        ImGui::Text("I am the last one here.");
                        ImGui::EndPopup();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
            ImGui::EndPopup();
        }
	}
}
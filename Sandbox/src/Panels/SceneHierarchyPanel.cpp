#include "SceneHierarchyPanel.h"
#include <glm/gtc/type_ptr.hpp>

namespace SGE {
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
		:m_SceneContext(scene) {}

	SceneHierarchyPanel::~SceneHierarchyPanel() {}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (!m_SceneContext)
			return;
		
		ImGui::Begin("Scene Hierarchy");
		ImGui::SetNextItemOpen(true);
		if (ImGui::TreeNode("Scene"))
		{
			if (m_align_label_with_current_x_position)
				ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

			auto view = m_SceneContext->Registry().view<TagComponent>();
			for(auto e : view)
			{
				DrawEntityNode(Entity{e, m_SceneContext.get()});
			}

			if (m_align_label_with_current_x_position)
				ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
			ImGui::TreePop();
		}
		ImGui::End();

		if(m_SelectedEntity)
			ShowComponents();
	}
	
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_SceneContext = scene;
	}
	
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
		// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
		ImGuiTreeNodeFlags node_flags = m_TreeNodeFlags;
		auto& tag = entity.GetComponent<TagComponent>();
		const bool is_selected = entity == m_SelectedEntity;

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
	
	void SceneHierarchyPanel::ShowComponents()
	{
		ImGui::Begin("Properties");

		if(m_SelectedEntity.HasComponent<TagComponent>())
		{
			auto& tag = m_SelectedEntity.GetComponent<TagComponent>();
			ImGui::Text("%s", tag.Tag.c_str());
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
		}

		ImGui::End();
	}
	
	
}
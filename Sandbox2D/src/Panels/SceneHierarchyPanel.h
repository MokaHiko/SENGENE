#ifndef SCENEHIERARCHYPANEL_H
#define SCENEHIERARCHYPANEL_H

#pragma once

#include <imgui/imgui.h>
#include <Scene/Scene.h>

namespace SGE{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);
        ~SceneHierarchyPanel();

        void OnImGuiRender();
        void SetContext(const Ref<Scene>& scene);
    public:
        void DrawEntityNode(Entity entity);
        
        void ShowMenu();
        void ShowSelectedComponents();
    private:   
        ImGuiTreeNodeFlags m_TreeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool m_align_label_with_current_x_position = false;
		bool m_test_drag_and_drop = false;

        void ScenePopupWindows();
        void EntityPopupWindows();
    private:
        Entity m_SelectedEntity;
        Ref<Scene> m_SceneContext; // TODO: SOME SORT OF WEAK PTR
    };
}

#endif
#ifndef FEATURETEST3D_H
#define FEATURETEST3D_H

#pragma once

#include "SGE/SGE.h"

#include "Renderer/Model.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Renderer/Framebuffer.h"

class FeatureTest3D: public SGE::Layer
{
public:
    FeatureTest3D() = default;
    ~FeatureTest3D();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(SGE::Event& event) override;

    virtual void OnUpdate(SGE::TimeStep ts) override;
    virtual void OnImGuiRender() override;

    glm::vec2 m_ViewPortSize = { 0.0f, 0.0f };
private:
    bool OnWindowResize(SGE::WindowResizeEvent& event);
private:
    SGE::SceneHierarchyPanel m_SceneHierarchyPanel;
    
    void ShowFileMenuHierarchy();
private:
    SGE::Ref<SGE::Framebuffer> m_Framebuffer;
    SGE::Ref<SGE::Scene> m_Scene;
    SGE::SceneData m_SceneData{};

    SGE::Ref<SGE::Texture2D> m_SampleTexture;

    SGE::Ref<SGE::Model> m_Model;
};

#endif
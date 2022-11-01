#ifndef Sandbox2D_H
#define Sandbox2D_H

#pragma once

#include "SGE/SGE.h"

#include "Renderer/Model.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Renderer/Framebuffer.h"

#include "Scene/SceneSerializer.h"

class Sandbox2D: public SGE::Layer
{
public:
    Sandbox2D() = default;
    ~Sandbox2D();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(SGE::Event& event) override;

    virtual void OnUpdate(SGE::TimeStep ts) override;
    virtual void OnImGuiRender() override;

    glm::vec2 m_ViewPortSize = { 0.0f, 0.0f };
private:
    void LoadScene(const std::string& filePath);
    void ResetScene();
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
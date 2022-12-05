#ifndef EDITORLAYER_H
#define EDITORLAYER_H

#pragma once

#include "SGE/SGE.h"

#include "Renderer/Model.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/DebugConsolePanel.h"
#include "Renderer/Framebuffer.h"
#include "Core/Input.h"

#include "Scene/SceneSerializer.h"

class EditorLayer: public SGE::Layer
{
public:
    EditorLayer() = default;
    ~EditorLayer();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(SGE::Event& event) override;

    virtual void OnUpdate(SGE::TimeStep ts) override;
    virtual void OnImGuiRender() override;

    glm::vec2 m_ViewPortSize = { 0.0f, 0.0f };

    // Editor Specific Inputs
    static glm::vec2 editorMouseInput;
private:
    void LoadScene(const std::string& filePath);
    void ResetScene();
    bool OnWindowResize(SGE::WindowResizeEvent& event);
private:
    SGE::SceneHierarchyPanel m_SceneHierarchyPanel;
    SGE::DebugConsolePanel m_DebugConsolePanel;
    
    void ShowFileMenuHierarchy();
    void ShowGameViewPort();
private:
    SGE::Ref<SGE::Framebuffer> m_Framebuffer;
    SGE::Ref<SGE::Scene> m_Scene;
    SGE::SceneData m_SceneData{};

    SGE::Ref<SGE::Texture2D> m_SampleTexture;

    SGE::Ref<SGE::Model> m_Model;
private:
    // - Widgets Data
    float m_FrameTime = 0;
};

#endif
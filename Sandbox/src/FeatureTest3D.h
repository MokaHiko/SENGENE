#ifndef FEATURETEST3D_H
#define FEATURETEST3D_H

#pragma once

#include "SGE/SGE.h"

#include "Renderer/Model.h"

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
private:
    SGE::Ref<SGE::Shader> m_SampleShader;
    SGE::Ref<SGE::Scene> m_Scene;

    SGE::SceneData m_SceneData{};

    SGE::Ref<SGE::Model> m_Model;
};

#endif
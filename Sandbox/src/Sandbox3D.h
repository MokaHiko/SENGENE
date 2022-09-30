#ifndef SANDBOX3D_H
#define SANDBOX3D_H

#pragma once

#include "SGE/SGE.h"

class Sandbox3D : public SGE::Layer
{
public:
    Sandbox3D() = default;
    ~Sandbox3D();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(SGE::Event& event) override;

    virtual void OnUpdate(SGE::TimeStep ts) override;
    virtual void OnImGuiRender() override;
private:
    SGE::Ref<SGE::Shader> m_SampleShader;
    SGE::Ref<SGE::Scene> m_Scene;

    SGE::Ref<SGE::Texture2D> testImage;
    SGE::Ref<SGE::Texture2D> testImage1;
    SGE::Ref<SGE::Texture2D> testImage2;

    SGE::SceneData m_SceneData{};
};

#endif
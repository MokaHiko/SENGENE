#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#pragma once

#include "SGE/SGE.h"
#include "SGE/Events/MouseEvent.h"

class CameraController : public SGE::ScriptableEntity
{
public:
    float MouseSensitivity = 0.1f;
    bool ConstrainPitch = true;

    SGE::Ref<SGE::Camera3D> m_Camera; // TODO:: Should be a weak ptr

    virtual void OnCreate() override
    {
        auto& watcher = AddComponent <SGE::EventWatcherComponent<SGE::MouseMoveEvent>>();
        watcher.Watch(std::bind(&CameraController::MouseMoveEventCallBack, this, std::placeholders::_1));
    }

    virtual void OnStart() override
    {
        std::cout << "Created Camera Controller" << std::endl;
        m_Camera = SGE::Ref<SGE::Camera3D>(&GetComponent<SGE::Camera3DComponent>().camera);
    }

    virtual void OnUpdate(SGE::TimeStep timestep) override
    {
        // Handle KeyBoard Input
        glm::vec3& cameraPosition = GetComponent<SGE::TransformComponent>().Position;
        if(SGE::Input::IsKeyPressed(GLFW_KEY_W))
                cameraPosition += m_Camera->GetFront() * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_S))
                cameraPosition -= m_Camera->GetFront() * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_D))
                cameraPosition += m_Camera->GetRight() * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_A))
                cameraPosition -= m_Camera->GetRight() * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_SPACE))
                cameraPosition += m_Camera->GetUp() * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
                cameraPosition -= m_Camera->GetUp() * timestep.GetSeconds();
    }

    void MouseMoveEventCallBack(SGE::MouseMoveEvent& event)
    {
        auto [x,y] = event.GetMouseCoordinates();
        if(m_FirstMove)
        {
            m_LastX = x;
            m_LastY = y;
            m_FirstMove = false;
        }

        float xOffset = x - m_LastX;
        float yOffset = m_LastY - y;
        
        m_LastX = x;
        m_LastY = y;

        ProcessMouseMovement(xOffset, yOffset);
    }

private:
    float m_LastX = 0;
    float m_LastY = 0;
    bool m_FirstMove = true;

    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        m_Camera->m_Yaw += xoffset;
        m_Camera->m_Pitch += yoffset;

        if(ConstrainPitch)
        {
            if(m_Camera->m_Pitch > 89.0f)
                m_Camera->m_Pitch = 89.0f;
            if(m_Camera->m_Pitch < -89.0f)
                m_Camera->m_Pitch = -89.0f;
        }
    }
};
#endif
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
    float MovementSpeed = 50.0f;

    SGE::Ref<SGE::Camera3D> m_Camera; // TODO:: Should be a weak ptr or something

    virtual void OnCreate() override
    {
        auto& watcher = AddComponent<SGE::EventWatcherComponent<SGE::MouseMoveEvent>>();
        watcher.Watch(std::bind(&CameraController::MouseMoveEventCallBack, this, std::placeholders::_1));
    }

    virtual void OnStart() override
    {
        std::cout << "Created Camera Controller" << std::endl;
        m_Camera = SGE::Ref<SGE::Camera3D>(&GetComponent<SGE::Camera3DComponent>().camera);
    }

    virtual void OnUpdate(SGE::TimeStep timestep) override
    {
        if (SGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) 
        {
            glm::vec3 rayDir = MouseToWorldCoordinates();
            auto ray = flg::Ray(GetComponent<SGE::TransformComponent>().Position, rayDir);
            flg::PhysicsWorld::Raycast(ray);
        }
        if (!SGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
            return;

        // Handle KeyBoard Input
        glm::vec3& cameraPosition = GetComponent<SGE::TransformComponent>().Position;
        if(SGE::Input::IsKeyPressed(GLFW_KEY_W))
                cameraPosition += m_Camera->GetFront() * MovementSpeed * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_S))
                cameraPosition -= m_Camera->GetFront() * MovementSpeed * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_D))
                cameraPosition += m_Camera->GetRight() * MovementSpeed * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_A))
                cameraPosition -= m_Camera->GetRight() * MovementSpeed * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_SPACE))
                cameraPosition += m_Camera->GetUp() * MovementSpeed * timestep.GetSeconds();
        if(SGE::Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
                cameraPosition -= m_Camera->GetUp() * MovementSpeed * timestep.GetSeconds();
    }

    glm::vec3 MouseToWorldCoordinates()
    {
        auto [x, y] = SGE::Input::GetMousePosition();
        glm::vec2 ndc = ScreenToNDC(x, y);
        glm::vec4 clip = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
        glm::vec4 eye = ClipToEye(clip);
        glm::vec3 world = glm::normalize(EyeToWorld(eye));
        return world;
    };

    glm::vec2 ScreenToNDC(float x, float y)
    {
        glm::vec2 ndc= {};
        float viewPortWidth = static_cast<float>(SGE::Renderer::GetSceneData().SceneWidth);
        float viewPortHeight = static_cast<float>(SGE::Renderer::GetSceneData().SceneHeight);
        ndc.x = (x * 2.0f) / viewPortWidth - 1.0f;
        ndc.y = (y * 2.0f) / viewPortHeight - 1.0f;
        ndc.y *= -1;
        return ndc;
    };

    glm::vec4 ClipToEye(glm::vec4& clip)
    {
        glm::vec4 eye = glm::inverse(SGE::Renderer::GetSceneData().ProjectionMatrix) * clip;
        eye.z = -1.0f;
        eye.w = 0.0f;
        return eye;
    };

    glm::vec3 EyeToWorld(const glm::vec4& eye)
    {
        glm::vec4 worldCoords = glm::inverse(m_Camera->GetViewMatrix()) * eye;
        return glm::vec3(worldCoords);
    }

    void MouseMoveEventCallBack(SGE::MouseMoveEvent& event)
    {
        if (!SGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
        {
            m_FirstMove = true;
            return;
        }

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
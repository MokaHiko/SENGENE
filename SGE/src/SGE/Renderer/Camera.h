#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace SGE {
    enum class Camera_Movement 
    {
        FORWARD,
        BACKWARD,
        UP,
        DOWN,
        LEFT, 
        RIGHT,
    };

    class Camera3D
    {
    public:
        Camera3D();
        ~Camera3D();

        glm::mat4 const GetViewMatrix() const;

        void CalculateViewMatrix(const glm::vec3& position);
        void UpdateCameraVectors();

        const glm::vec3& GetFront() {return m_Front;}
        const glm::vec3& GetRight() {return m_Right;}
        const glm::vec3& GetUp() {return m_Up;}
    public:
        // Camera Relative Position Attributes
        glm::vec3 m_Front;
        glm::vec3 m_Right;
        glm::vec3 m_Up;
        glm::vec3 m_WorldUp = glm::vec3(0, 1, 0);

        // Camera Settings
        float m_Yaw = -90.0f;
        float m_Pitch = 0.0f;

        float m_Speed = 2.5f;
        float m_Zoom = 45.0f;
        float m_Sensitivity = 0.1f;
    private:
        glm::mat4 m_ViewMatrix;
    };
}

#endif
#include "Camera.h"

namespace SGE {
	Camera3D::Camera3D()
	{
		UpdateCameraVectors();
	}
	
	Camera3D::~Camera3D()
	{
	}

	glm::mat4 const Camera3D::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}
	
	void Camera3D::CalculateViewMatrix(const glm::vec3& position)
	{
		m_ViewMatrix = glm::lookAt(position, position + m_Front, m_Up);
	}
	
	void Camera3D::UpdateCameraVectors()
	{
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);

        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
	}
}
#include "FlyCamera.h"

FlyCamera::FlyCamera()
{
	m_Speed = 20.0f;
}

void FlyCamera::update(float a_DeltaTime)
{
	Camera::update(a_DeltaTime);
	
	glfwGetCursorPos(glfwGetCurrentContext(), &m_MousePosX, &m_MousePosY);

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_Position.z -= m_Speed * a_DeltaTime;
	}
}
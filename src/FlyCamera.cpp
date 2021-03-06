#include "FlyCamera.h"
#include "TweakBar.h"

FlyCamera::FlyCamera()
{
	m_Speed = 20.0f;
	m_Direction = vec3();
	int width, height;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
	glfwSetCursorPos(glfwGetCurrentContext(), width / 2, height / 2);
	m_Timer = 0;
	m_EnableMouse = false;
	m_keyDown = false;
}

void FlyCamera::update(float a_DeltaTime)
{
	m_Timer += a_DeltaTime;
	int width, height;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
		
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS && m_keyDown == false)
	{
		m_EnableMouse = !m_EnableMouse;
		glfwSetCursorPos(glfwGetCurrentContext(), width / 2, height / 2);
		m_keyDown = true;
	}	

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_RELEASE && m_keyDown == true)
	{
		m_keyDown = false;
	}

	glfwGetCursorPos(glfwGetCurrentContext(), &m_MousePosX, &m_MousePosY);

	if (m_MousePosX != width / 2 && m_EnableMouse)
	{
		float diff = m_MousePosX - width / 2;
		m_WorldTransform = m_WorldTransform * glm::rotate(-diff * a_DeltaTime, vec3(0, 1, 0));
	}
	if (m_MousePosY != height / 2 && m_EnableMouse)
	{
		float diff = m_MousePosY - height / 2;
		m_WorldTransform = m_WorldTransform * glm::rotate(-diff * a_DeltaTime, vec3(1, 0, 0));
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	{
		setPosition(vec3(-m_Speed * a_DeltaTime, 0, 0));
		//m_WorldTransform = m_WorldTransform * glm::translate(vec3(-m_Speed * a_DeltaTime, 0, 0));
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	{
		setPosition(vec3(m_Speed * a_DeltaTime, 0, 0));
		//m_WorldTransform = m_WorldTransform * glm::translate(vec3(m_Speed * a_DeltaTime, 0, 0));
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		setPosition(vec3(0, 0, -m_Speed * a_DeltaTime));
		//m_WorldTransform = m_WorldTransform * glm::translate(vec3(0, 0, -m_Speed * a_DeltaTime));
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		setPosition(vec3(0, 0, m_Speed * a_DeltaTime));
		//m_WorldTransform = m_WorldTransform * glm::translate(vec3(0, 0, m_Speed * a_DeltaTime));
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS)
	{
		setPosition(vec3(0, -m_Speed * a_DeltaTime, 0));
		//m_WorldTransform = m_WorldTransform * glm::translate(vec3(0, -m_Speed * a_DeltaTime, 0));
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS)
	{
		setPosition(vec3(0, m_Speed * a_DeltaTime, 0));
		//m_WorldTransform = m_WorldTransform * glm::translate(vec3(0, m_Speed * a_DeltaTime, 0));
	}
	
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_R) == GLFW_PRESS)
	{
		m_WorldTransform = m_WorldTransform * glm::rotate(m_Speed * a_DeltaTime * 0.1f, vec3(0, 0, 1));
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_F) == GLFW_PRESS)
	{
		m_WorldTransform = m_WorldTransform * glm::rotate(-m_Speed * a_DeltaTime * 0.1f, vec3(0, 0, 1));
	}

	Camera::update(a_DeltaTime);
	
	if (m_EnableMouse)
		glfwSetCursorPos(glfwGetCurrentContext(), width / 2, height / 2);

}
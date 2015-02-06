#include "baseCamera.h"

Camera::Camera()
{
	m_ProjectionViewTransform = glm::perspective(glm::radians(60.0f), 16 / 9.f, 0.1f, 1000.f);	m_ViewTransform = glm::lookAt(m_Position, vec3(0, 0, 0), vec3(0, 1, 0));	m_WorldTransform = glm::inverse(m_ViewTransform);	m_ProjectionTransform = glm::inverse(m_ProjectionViewTransform);	
}

void Camera::update(float a_DeltaTime)
{	

}

void Camera::setPerspective(float a_FieldOfView, float a_Near, float a_Far)
{
	m_ProjectionViewTransform = glm::perspective(glm::radians(a_FieldOfView), 16 / 9.f, a_Near, a_Far);
}

void Camera::setLookAt(vec3 a_From, vec3 a_To, vec3 a_Up)
{
	m_ViewTransform = glm::lookAt(a_From, a_To, a_Up);
}

void Camera::setPosition(vec3 a_Position)
{
	m_Position = a_Position;
}

mat4 Camera::getWorldTransform()
{
	return m_WorldTransform;
}

mat4 Camera::getView()
{
	return m_ViewTransform;
}

mat4 Camera::getProjection()
{
	return m_ProjectionTransform;
}

mat4 Camera::getProjectionView()
{
	return m_ProjectionViewTransform;
}

void Camera::updateProjectionViewTransform()
{

}
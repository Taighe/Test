#include "CameraProjection.h"
#include "FlyCamera.h"

CameraProjection::CameraProjection()
{
	m_Camera = new FlyCamera();
}

bool CameraProjection::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	Gizmos::create();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	return true;
}

bool CameraProjection::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	
	float dt = glfwGetTime();
	glfwSetTime(0.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	vec4 m_White = vec4(1);
	vec4 m_Black = vec4(0, 0, 0, 1);
	Gizmos::addTransform(mat4(1));
	
	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? m_White : m_Black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? m_White : m_Black);
	}
	
	m_Camera->update(dt);
	

	return true;
}

void CameraProjection::draw()
{
	Gizmos::draw(m_Camera->getProjectionView());
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void CameraProjection::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}
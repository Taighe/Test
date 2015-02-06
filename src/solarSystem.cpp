#define GLM_SWIZZLE

#include "solarSystem.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <cstdio>
#include "Gizmos.h"


SolarSystem::SolarSystem()
{
	m_Projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	m_White = vec4(1);
	m_Yellow = vec4(1, 0.8, 0.1, 1);
	m_Black = vec4(0, 0, 0, 1);
	m_CameraPos = vec3(10, 10, 10);
	m_Rotate = 0;
}

mat4 SolarSystem::BuildOrbitMatrix(float local_rotation, float radius, float orbit_rotation)
{
	mat4 result = glm::rotate(orbit_rotation, vec3(0, 1, 0)) * glm::translate(vec3(radius, 0, 0)) * glm::rotate(local_rotation, vec3(0, 1, 0));

	return result;
}

bool SolarSystem::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
		
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	Gizmos::create();

	return true;
}

bool SolarSystem::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	
	float dt = glfwGetTime();
	glfwSetTime(0.0f);
	m_Timer += dt / 3;

	//m_CameraPos.x = sinf() * 20;
	//m_CameraPos.z = cosf(m_Timer) * 20;
	m_View = glm::lookAt(m_CameraPos, vec3(0, 0, 0), vec3(0, 1, 0));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	Gizmos::addTransform(mat4(1));


	mat4 sun = BuildOrbitMatrix(m_Timer, 0, 0);
	//mat4 transform = glm::tr
	for (int i = 0; i <= 20; i++) 
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? m_White : m_Black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? m_White : m_Black);
	}

	//Gizmos::addAABB(vec3(0, 0, 0), vec3(1, 1, 1), vec4(0, 1, 0, 1));
	//Gizmos::addAABBFilled(vec3(0, 0, 0), vec3(1, 1, 1), vec4(0, 1, 0, 1));
	Gizmos::addSphere(sun[3].xyz, 3.0f, 10, 10, m_Yellow, &sun);
	for (int i = 0; i < 10; i++)
	{
		mat4 planetMatrix = sun * BuildOrbitMatrix(m_Timer / (float)i, 2 * i, m_Timer * (float)i);
		Gizmos::addSphere(planetMatrix[3].xyz, 0.5f, 10, 10, m_White, &planetMatrix);
	}



	return true;
}

void SolarSystem::draw()
{
	Gizmos::draw(m_Projection, m_View);
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void SolarSystem::shutdown()
{
	Gizmos::destroy();
	
	Application::shutdown();
}
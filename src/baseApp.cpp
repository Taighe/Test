#include "baseApp.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <cstdio>

Application::Application()
{
	m_WindowWidth = 1280;
	m_WindowHeight = 720;
}

Application::~Application(){}


bool Application::startup()
{
	if (glfwInit() == false)
	{
		return false;
	}

	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "Computer Graphics", nullptr, nullptr);

	if (m_Window == nullptr)
	{
		return false;
	}	

	glfwMakeContextCurrent(m_Window);

	if(	ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
		return false;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();
	printf("Loaded OpenGl version %d.%d\n", major_version, minor_version);

	return true;
}

bool Application::update()
{
	if (glfwWindowShouldClose(m_Window) == false)
	{
		return true;
	}
	
	return false;
	
}
void Application::draw()
{

}

void Application::shutdown()
{
	glfwDestroyWindow(m_Window);

	glfwTerminate();
}
#define GLM_SWIZZLE

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <cstdio>
#include "Gizmos.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"


using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

mat4 BuildOrbitMatrix(float local_rotation, float radius, float orbit_rotation)
{
	mat4 result = glm::rotate(orbit_rotation, vec3(0, 1, 0)) * glm::translate(vec3(radius, 0, 0)) * glm::rotate(local_rotation, vec3(0, 1, 0));

	return result;
}

int main()
{
	if (glfwInit() == false)
	{
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	if (window == nullptr)
	{
		return -2;
	}	

	glfwMakeContextCurrent(window);

	if(	ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();
	printf("Loaded OpenGl version %d.%d\n", major_version, minor_version);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	Gizmos::create();

	vec3 camPos = vec3(0, 20, 10);
	float timer = 0.0f;
	vec4 white(1);
	vec4 yellow(1, 0.8, 0.1, 1);
	vec4 black(0, 0, 0, 1);
	
	mat4 projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	while (glfwWindowShouldClose(window) == false)
	{
		float dt = glfwGetTime();
		glfwSetTime(0.0f);
		timer += dt / 3;
		camPos.x = sinf(timer) * 20;
		camPos.z = cosf(timer) * 20;
		mat4 view = glm::lookAt(camPos, vec3(0, 0, 0), vec3(0, 1, 0));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Gizmos::clear();
		Gizmos::addTransform(mat4(1));


		mat4 sun = BuildOrbitMatrix(timer, 0, 0);
		//mat4 transform = glm::tr
		for (int i = 0; i <= 20; i++)
		{
			Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? white : black);
			Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
		}

		//Gizmos::addAABB(vec3(0, 0, 0), vec3(1, 1, 1), vec4(0, 1, 0, 1));
		//Gizmos::addAABBFilled(vec3(0, 0, 0), vec3(1, 1, 1), vec4(0, 1, 0, 1));
		Gizmos::addSphere(sun[3].xyz, 3.0f, 10, 10, yellow, &sun);
		for (int i = 0; i < 10; i++)
		{
			mat4 planetMatrix = sun * BuildOrbitMatrix(timer / (float)i, 2 * i, timer * (float)i);
			Gizmos::addSphere(planetMatrix[3].xyz, 0.5f, 10, 10, white, &planetMatrix);
		}



		Gizmos::draw(projection, view);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	Gizmos::destroy();
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
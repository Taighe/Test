#include "LightingApp.h"
#include "FlyCamera.h"
#include "glm_header.h"
#include "Gizmos.h"

#include "FBXFile.h"


LightingApp::LightingApp()
{
	m_Camera = new FlyCamera();
}

bool LightingApp::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	Gizmos::create();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	m_File = new FBXFile();
	m_File->load("./models/stanford/Bunny.fbx");

	createOpenGLBuffers(m_File);

	return true;
}

bool LightingApp::update()
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

void LightingApp::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(m_ProgramID);
	
	int projViewHandle = glGetUniformLocation(m_ProgramID, "projectionView");

	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, false, (float*)&m_Camera->getProjectionView());
	}

	for (unsigned int meshIndex = 0; meshIndex < m_File->getMeshCount(); meshIndex++)
	{
		FBXMeshNode* mesh = m_File->getMeshByIndex(meshIndex);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
	}

	Gizmos::draw(m_Camera->getProjectionView());
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void LightingApp::shutdown()
{
	cleanupOpenGLBuffers(m_File);
	
	Gizmos::destroy();

	Application::shutdown();
}

void LightingApp::createOpenGLBuffers(FBXFile* a_File)
{
	for (unsigned int meshIndex = 0; meshIndex < a_File->getMeshCount; ++meshIndex)
	{
		FBXMeshNode* mesh = a_File->getMeshByIndex(meshIndex);
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glGenBuffers(2, &glData[1]);

		glBindVertexArray(glData[0]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)(FBXVertex::NormalOffset));
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;

	}
}

void LightingApp::cleanupOpenGLBuffers(FBXFile* a_File)
{
	for (unsigned int meshIndex = 0; meshIndex < a_File->getMeshCount; ++meshIndex)
	{
		FBXMeshNode* mesh = a_File->getMeshByIndex(meshIndex);
		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);
		delete[] glData;
	}
}

void LightingApp::reloadShader()
{
	glDeleteProgram(m_ProgramID);
	loadShader();
}
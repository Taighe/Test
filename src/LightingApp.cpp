#include "LightingApp.h"
#include "FlyCamera.h"
#include "Gizmos.h"

#include "Utility.h"


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

	loadShaders("./shaders/lightingVertex.glsl", "./shaders/lightingFragment.glsl", &m_ProgramID);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, "./models/bunny.obj");

	if (err.size() != 0)
	{
		return false;
	}

	createOpenGLBuffers(shapes);

	m_ambientLight = vec3(0.1, 0.1, 0.1);
	m_lightDir = vec3(0, -1, 0);
	m_lightColor = vec3(1, 1, 1);
	m_materialColor = vec3(1, 1, 0);
	m_specularPower = 100;

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
	m_Timer += dt;

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_1) == GLFW_PRESS)
	{
		reloadShader();
	}

	unsigned int ambientUniform = glGetUniformLocation(m_ProgramID, "ambientLight");
	unsigned int lightDirUniform = glGetUniformLocation(m_ProgramID, "lightDir");
	unsigned int lightColorUniform = glGetUniformLocation(m_ProgramID, "lightColor");
	unsigned int materialColorUniform = glGetUniformLocation(m_ProgramID, "materialColor");
	unsigned int specularPowerUniform = glGetUniformLocation(m_ProgramID, "specularPower");
	unsigned int eyePosUniform = glGetUniformLocation(m_ProgramID, "eyePos");

	glUniform3fv(ambientUniform, 1, (float*)&m_ambientLight);
	glUniform3fv(lightDirUniform, 1, (float*)&m_lightDir);
	glUniform3fv(lightColorUniform, 1, (float*)&m_lightColor);
	glUniform3fv(materialColorUniform, 1, (float*)&m_materialColor);
	glUniform1f(specularPowerUniform, m_specularPower);
	
	vec3 cameraPos = m_Camera->getWorldTransform()[3].xyz;
	glUniform3fv(eyePosUniform, 1, (float*)&cameraPos);

	unsigned int timerHandle = glGetUniformLocation(m_ProgramID, "timer");
	glUniform1f(timerHandle, m_Timer);

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
	
	for (unsigned int meshIndex = 0; meshIndex < m_gl_data.size(); ++meshIndex)
	{
		glBindVertexArray(m_gl_data[meshIndex].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_data[meshIndex].m_IndexCount, GL_UNSIGNED_INT, 0);
	}

	Gizmos::draw(m_Camera->getProjectionView());
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void LightingApp::shutdown()
{
	cleanupOpenGLBuffers();
	
	Gizmos::destroy();

	Application::shutdown();
}

//void LightingApp::createOpenGLBuffers(FBXFile* a_File)
//{
//	for (unsigned int meshIndex = 0; meshIndex < a_File->getMeshCount(); ++meshIndex)
//	{
//		FBXMeshNode* mesh = a_File->getMeshByIndex(meshIndex);
//		unsigned int* glData = new unsigned int[3];
//
//		glGenVertexArrays(1, &glData[0]);
//		glGenBuffers(2, &glData[1]);
//
//		glBindVertexArray(glData[0]);
//
//		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
//		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//
//		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
//		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)(FBXVertex::NormalOffset));
//		
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		mesh->m_userData = glData;
//
//	}
//}

//void LightingApp::cleanupOpenGLBuffers()
//{
//	for (unsigned int meshIndex = 0; meshIndex < a_File->getMeshCount(); ++meshIndex)
//	{
//		FBXMeshNode* mesh = a_File->getMeshByIndex(meshIndex);
//		unsigned int* glData = (unsigned int*)mesh->m_userData;
//
//		glDeleteVertexArrays(1, &glData[0]);
//		glDeleteBuffers(1, &glData[1]);
//		glDeleteBuffers(1, &glData[2]);
//		delete[] glData;
//
//	}
//}

void LightingApp::reloadShader()
{
	glDeleteProgram(m_ProgramID);
	loadShaders("./shaders/lightingVertex.glsl", "./shaders/lightingFragment.glsl", &m_ProgramID);
}

void LightingApp::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_data.resize(shapes.size());

	for (unsigned int shapeIndex = 0; shapeIndex < shapes.size(); ++shapeIndex)
	{
		std::vector<float> vertexData;

		unsigned int floatCount = shapes[shapeIndex].mesh.positions.size();
		floatCount += shapes[shapeIndex].mesh.normals.size();

		vertexData.reserve(floatCount);

		vertexData.insert(vertexData.end(), shapes[shapeIndex].mesh.positions.begin(), shapes[shapeIndex].mesh.positions.end());
		vertexData.insert(vertexData.end(), shapes[shapeIndex].mesh.normals.begin(), shapes[shapeIndex].mesh.normals.end());

		m_gl_data[shapeIndex].m_IndexCount = shapes[shapeIndex].mesh.indices.size();

		glGenVertexArrays(1, &m_gl_data[shapeIndex].m_VAO);
		glGenBuffers(1, &m_gl_data[shapeIndex].m_VBO);
		glGenBuffers(1, &m_gl_data[shapeIndex].m_IBO);

		glBindVertexArray(m_gl_data[shapeIndex].m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_data[shapeIndex].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, vertexData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_data[shapeIndex].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[shapeIndex].mesh.indices.size() * sizeof(unsigned int), shapes[shapeIndex].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float) * shapes[shapeIndex].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}
}

void LightingApp::cleanupOpenGLBuffers()
{
	glDeleteProgram(m_ProgramID);
	for (unsigned int i = 0; i < m_gl_data.size(); ++i)
	{
		glDeleteVertexArrays(1, &m_gl_data[i].m_VAO);
		glDeleteBuffers(1, &m_gl_data[i].m_VBO);
		glDeleteBuffers(1, &m_gl_data[i].m_IBO);
	}
}

#include "RenderGeo.h"
#include "Vertex.h"

#include "glm_header.h"
#include "Gizmos.h"
#include <cstdio>


bool RenderingGeometry::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	
	generateShader();
	generateGrid(10, 10);

	Gizmos::create();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	m_Camera = new FlyCamera();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_Timer = 0;
	return true;
}

bool RenderingGeometry::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = glfwGetTime();
	glfwSetTime(0.0f);
	m_Timer += dt;
	glUseProgram(m_ProgramID);
	unsigned int timerHandle = glGetUniformLocation(m_ProgramID, "timer");
	glUniform1f(timerHandle, m_Timer);

	m_Camera->update(dt);
	vec4 m_White = vec4(1);
	vec4 m_Black = vec4(0, 0, 0, 1);
	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? m_White : m_Black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? m_White : m_Black);
	}
	return true;

}

void RenderingGeometry::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_ProgramID);
	int projViewHandle = glGetUniformLocation(m_ProgramID, "projectionView");
	
	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, false, (float*)&m_Camera->getProjectionView());
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
	
	Gizmos::draw(m_Camera->getProjectionView());
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void RenderingGeometry::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

void RenderingGeometry::generateGrid(unsigned int a_Rows, unsigned int a_Cols)
{
	
	Vertex* vertext_array = new Vertex[(a_Rows + 1) * (a_Cols + 1)];

	for (unsigned int r = 0; r < a_Rows + 1; ++r)
	{
		for (unsigned int c = 0; c < a_Cols + 1; ++c)
		{
			vertext_array[c + r * (a_Cols + 1)].position = vec4((float)c, 0, (float)r, 1);
			vertext_array[c + r * (a_Cols + 1)].color = vec4((float)c / (a_Cols + 1), 0, (float)r / (a_Rows + 1), 1);
		}
	}

	m_IndexCount = a_Rows * a_Cols * 6;
	unsigned int* index_array = new unsigned int[a_Rows * a_Cols * 6];

	int index_location = 0;

	for (unsigned int r = 0; r < a_Rows; ++r)
	{
		for (unsigned int c = 0; c < a_Cols; ++c)
		{
			index_array[index_location + 0] = c + r * (a_Cols + 1);
			index_array[index_location + 1] = c + (r + 1) * (a_Cols + 1);
			index_array[index_location + 2] = (c + 1) + r * (a_Cols + 1);

			index_array[index_location + 3] = (c + 1) + r * (a_Cols + 1);
			index_array[index_location + 4] = c + (r + 1) * (a_Cols + 1);
			index_array[index_location + 5] = (c + 1) + (r + 1) * (a_Cols + 1);

			index_location += 6;
		}
	}
	
	//Creating the buffers
	glGenBuffers(1, &m_VBO); //vertex buffer object
	glGenBuffers(1, &m_IBO);//index buffer object
	glGenVertexArrays(1, &m_VAO); //vertex array object

	glBindVertexArray(m_VAO); 

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, (a_Cols + 1) * (a_Rows + 1) * sizeof(Vertex), vertext_array, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); // color

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_Rows * a_Cols * 6 * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	//unbinding buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertext_array;
	delete[] index_array;
}

void RenderingGeometry::generateShader()
{
	const char* vsSource = "#version 410\n "
							"layout(location=0) in vec4 position;\n "
							"layout(location=1) in vec4 color;\n "
							"out vec4 vColor;\n "
							"uniform mat4 projectionView;\n "
							"uniform float timer;\n "
							"void main() { vColor = color; gl_Position = projectionView * position;}\n ";

	const char* fsSource = "#version 410\n "
							"in vec4 vColor;\n"
							"out vec4 fragColor;\n"
							"void main() {fragColor = vColor; }\n";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, &fsSource, 0);
	glCompileShader(fragmentShader);
	
	m_ProgramID = glCreateProgram();

	glAttachShader(m_ProgramID, vertexShader);
	glAttachShader(m_ProgramID, fragmentShader);
	glLinkProgram(m_ProgramID);
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int length = 0;
		char* infoLog = new char[length];
		glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &length);
		glGetProgramInfoLog(m_ProgramID, length, 0, infoLog);
		printf("ERROR: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}
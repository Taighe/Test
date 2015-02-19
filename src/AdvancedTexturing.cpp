#include "AdvancedTexturing.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include "stb_image.h"
#include "Utility.h"


void onMouseButton(GLFWwindow* a_Window, int a_Button, int a_Pressed, int a_AltKeys)
{
	TwEventMouseButtonGLFW(a_Button, a_Pressed);
}

void onMousePosition(GLFWwindow* a_Window, double a_X, double a_Y)
{
	TwEventMousePosGLFW((int)a_X, (int)a_Y);
}

void onMouseScroll(GLFWwindow* a_Window, double a_X, double a_Y)
{
	TwEventMouseWheelGLFW((int)a_Y);
}

void onKey(GLFWwindow* a_Window, int a_Key, int a_Scancode, int a_Pressed, int a_AltKeys)
{
	TwEventKeyGLFW(a_Key, a_Pressed);
}

void onChar(GLFWwindow* a_Window, unsigned int a_C)
{
	TwEventCharGLFW(a_C, GLFW_PRESS);
}

void onWindowResize(GLFWwindow* a_Window, int a_Width, int a_Height)
{
	TwWindowSize(a_Width, a_Height);
	glViewport(0, 0, a_Width, a_Height);
}

AdvancedTexturing::AdvancedTexturing()
{
	m_Camera = new FlyCamera();
}

bool AdvancedTexturing::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glfwSetMouseButtonCallback(m_Window, onMouseButton);
	glfwSetCursorPosCallback(m_Window, onMousePosition);
	glfwSetScrollCallback(m_Window, onMouseScroll);
	glfwSetKeyCallback(m_Window, onKey);
	glfwSetCharCallback(m_Window, onChar);
	glfwSetWindowSizeCallback(m_Window, onWindowResize);
	
	Gizmos::create();

	glEnable(GL_DEPTH_TEST);
	
	generateQuad(5.0f);
	
	loadTexture("./textures/rock_diffuse.tga", "./textures/rock_normal.tga", "./textures/rock_specular.tga");
	loadShaders("./shaders/normalMappedVertex.glsl", "./shaders/normalMappedFragment.glsl", &m_ProgramID);

	m_ambientLight = vec3(0.1f);
	m_lightDir = (vec3(0, -1, -1));
	m_lightColor = vec3(1.0f);
	m_specularPower = 15.0f;
	m_GizmoSize = 10;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(m_WindowWidth, m_WindowHeight);
	m_Bar = TwNewBar("My Bar");
	TwAddVarRW(m_Bar, "Clear Color", TW_TYPE_COLOR4F, &m_backgroundColor, "");
	TwAddVarRW(m_Bar, "Direction", TW_TYPE_DIR3F, &m_lightDir, "group=Light");
	TwAddVarRW(m_Bar, "Color", TW_TYPE_COLOR4F, &m_lightColor, "group=Light");
	TwAddVarRW(m_Bar, "Specular Power", TW_TYPE_FLOAT, &m_specularPower, "group=Light min=0.1 max=100 step=0.5");
	TwAddVarRW(m_Bar, "Size", TW_TYPE_INT32, &m_GizmoSize, "group=Gizmo min=0 step=2");

	TwAddVarRO(m_Bar, "FPS", TW_TYPE_FLOAT, &m_FPS, "");
	m_backgroundColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);

	return true;
}

bool AdvancedTexturing::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	

	float dt = glfwGetTime();
	m_FPS = 1 / dt;
	glfwSetTime(0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	vec4 m_White = vec4(1);
	vec4 m_Black = vec4(0, 0, 0, 1);
	Gizmos::addTransform(mat4(1));
	

	for (int i = 0; i <= m_GizmoSize; i++)
	{
		Gizmos::addLine(vec3(-m_GizmoSize / 2 + i, -1, -m_GizmoSize / 2), vec3(-m_GizmoSize / 2 + i, -1, m_GizmoSize / 2), i == m_GizmoSize / 2 ? m_White : m_Black);
		Gizmos::addLine(vec3(-m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), vec3(m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), i == m_GizmoSize / 2 ? m_White : m_Black);
	}

	//m_lightDir = (glm::rotate(dt, vec3(0, 1, 0)) * vec4(m_lightDir, 0)).xyz;

	m_Camera->update(dt);

	return true;
}

void AdvancedTexturing::draw()
{
	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w);
	glUseProgram(m_ProgramID);
	int projViewHandle = glGetUniformLocation(m_ProgramID, "projectionView");

	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, false, (float*)&m_Camera->getProjectionView());
	}
	
	unsigned int ambientUniform = glGetUniformLocation(m_ProgramID, "ambientLight");
	unsigned int lightDirUniform = glGetUniformLocation(m_ProgramID, "lightDir");
	unsigned int lightColorUniform = glGetUniformLocation(m_ProgramID, "lightColor");
	unsigned int specularPowerUniform = glGetUniformLocation(m_ProgramID, "specularPower");
	unsigned int eyePosUniform = glGetUniformLocation(m_ProgramID, "eyePos");

	glUniform3fv(ambientUniform, 1, (float*)&m_ambientLight);
	glUniform3fv(lightDirUniform, 1, (float*)&m_lightDir);
	glUniform3fv(lightColorUniform, 1, (float*)&m_lightColor);
	

	vec3 cameraPos = m_Camera->getWorldTransform()[3].xyz;
	glUniform3fv(eyePosUniform, 1, (float*)&cameraPos);
	glUniform1f(specularPowerUniform, m_specularPower);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DiffuseTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NormalTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_SpecularTexture);

	int diffuseLocation = glGetUniformLocation(m_ProgramID, "diffuseTex");
	int normalLocation = glGetUniformLocation(m_ProgramID, "normalTex");
	int specularLocation = glGetUniformLocation(m_ProgramID, "specularTex");
	
	glUniform1i(diffuseLocation, 0);
	glUniform1i(normalLocation, 1);
	glUniform1i(specularLocation, 2);

	glBindVertexArray(m_Quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	TwDraw();
	Gizmos::draw(m_Camera->getProjectionView());
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void AdvancedTexturing::shutdown()
{
	Gizmos::destroy();

	TwDeleteAllBars();
	TwTerminate();

	Application::shutdown();
}

void AdvancedTexturing::generateQuad(float a_Size)
{
	nVertex vertex_data[4];

	vertex_data[0].position = vec4(-a_Size, 0, -a_Size, 1);
	vertex_data[1].position = vec4(-a_Size, 0, a_Size, 1);
	vertex_data[2].position = vec4(a_Size, 0, a_Size, 1);
	vertex_data[3].position = vec4(a_Size, 0, -a_Size, 1);

	vertex_data[0].normal = vec4(0, 1, 0, 0);
	vertex_data[1].normal = vec4(0, 1, 0, 0);
	vertex_data[2].normal = vec4(0, 1, 0, 0);
	vertex_data[3].normal = vec4(0, 1, 0, 0);
	
	vertex_data[0].tangent = vec4(1, 0, 0, 0);
	vertex_data[1].tangent = vec4(1, 0, 0, 0);
	vertex_data[2].tangent = vec4(1, 0, 0, 0);
	vertex_data[3].tangent = vec4(1, 0, 0, 0);


	vertex_data[0].texCoord = vec2(0, 0);
	vertex_data[1].texCoord = vec2(0, 1);
	vertex_data[2].texCoord = vec2(1, 1);
	vertex_data[3].texCoord = vec2(1, 0);

	unsigned int index_data[6] = { 0, 1, 2, 0, 2, 3 };
	m_Quad.m_IndexCount = 6;


	//Creating the buffers
	glGenBuffers(1, &m_Quad.m_VBO); //vertex buffer object
	glGenBuffers(1, &m_Quad.m_IBO);//index buffer object
	glGenVertexArrays(1, &m_Quad.m_VAO); //vertex array object

	glBindVertexArray(m_Quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(nVertex) * 4, vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Quad.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); // normal
	glEnableVertexAttribArray(2); // tangent
	glEnableVertexAttribArray(3); // tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(nVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(nVertex), (void*)(sizeof(vec4) * 1));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(nVertex), (void*)(sizeof(vec4) * 2));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(nVertex), (void*)(sizeof(vec4) * 3));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void AdvancedTexturing::loadTexture(char* a_DiffuseTexture, char* a_NormalTexture, char* a_SpecularTexture)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(a_DiffuseTexture, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_DiffuseTexture);
	glBindTexture(GL_TEXTURE_2D, m_DiffuseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load(a_NormalTexture, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_NormalTexture);
	glBindTexture(GL_TEXTURE_2D, m_NormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load(a_SpecularTexture, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_SpecularTexture);
	glBindTexture(GL_TEXTURE_2D, m_SpecularTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}
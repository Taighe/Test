#include "GPUParticleApp.h"

#include "FlyCamera.h"
#include "Gizmos.h"
#include "stb_image.h"
#include "Utility.h"
#include "glm_header.h"


GPUParticleApp::GPUParticleApp()
{
	m_Camera = new FlyCamera();
}

bool GPUParticleApp::startup()
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

	//generateQuad(0);

	//loadTexture("./textures/rock_diffuse.tga", "./textures/rock_normal.tga", "./textures/rock_specular.tga");
	//loadShaders("./shaders/particleVertex.glsl", "./shaders/particleFragment.glsl", &m_ProgramID);

	m_Timer = 0;

	m_ambientLight = vec3(0.1f);
	m_lightDir = (vec3(0, -1, -1));
	m_lightColor = vec3(1.0f);
	m_specularPower = 15.0f;
	m_GizmoSize = 10;
	m_emitter = new GPUEmitter();

	m_emitter->Init(10, 1, 10, 0.1, 10, 1, 0.1, vec4(1, 0, 0, 1), vec4(1, 1, 0, 1));

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(m_WindowWidth, m_WindowHeight);
	m_Bar = TwNewBar("My Bar");

	TwAddVarRW(m_Bar, "Clear Color", TW_TYPE_COLOR4F, &m_backgroundColor, "");

	TwAddVarRW(m_Bar, "Size", TW_TYPE_INT32, &m_GizmoSize, "group=Gizmo min=0 step=2");
	TwAddVarRO(m_Bar, "FPS", TW_TYPE_FLOAT, &m_FPS, "");

	m_backgroundColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);

	return true;
}

bool GPUParticleApp::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = glfwGetTime();
	m_FPS = 1 / dt;
	m_Timer += dt;
	glfwSetTime(0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	vec4 m_White = vec4(1);
	vec4 m_Black = vec4(0, 0, 0, 1);
	//Gizmos::addTransform(mat4(1));


	for (int i = 0; i <= m_GizmoSize; i++)
	{
		Gizmos::addLine(vec3(-m_GizmoSize / 2 + i, -1, -m_GizmoSize / 2), vec3(-m_GizmoSize / 2 + i, -1, m_GizmoSize / 2), i == m_GizmoSize / 2 ? m_White : m_Black);
		Gizmos::addLine(vec3(-m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), vec3(m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), i == m_GizmoSize / 2 ? m_White : m_Black);
	}

	//m_lightDir = (glm::rotate(dt, vec3(0, 1, 0)) * vec4(m_lightDir, 0)).xyz;

	m_Camera->update(dt);
	

	return true;
}

void GPUParticleApp::draw()
{
	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w);

	m_emitter->Draw((float)glfwGetTime(), m_Camera->getWorldTransform(), m_Camera->getProjectionView());
	TwDraw();

	Gizmos::draw(m_Camera->getProjectionView());

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void GPUParticleApp::shutdown()
{
	Gizmos::destroy();

	TwDeleteAllBars();
	TwTerminate();

	Application::shutdown();
}

void GPUParticleApp::generateQuad(float a_Size)
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(nVertex)* 4, vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Quad.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); // normal
	glEnableVertexAttribArray(2); // tangent
	glEnableVertexAttribArray(3); // tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(nVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(nVertex), (void*)(sizeof(vec4)* 1));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(nVertex), (void*)(sizeof(vec4)* 2));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(nVertex), (void*)(sizeof(vec4)* 3));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GPUParticleApp::loadTexture(char* a_DiffuseTexture, char* a_NormalTexture, char* a_SpecularTexture)
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
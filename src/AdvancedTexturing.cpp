#include "AdvancedTexturing.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include "stb_image.h"
#include "Utility.h"

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

	Gizmos::create();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	
	generateQuad(5.0f);
	
	loadTexture("./textures/rock_diffuse.tga", "./textures/rock_diffuse.tga", "./textures/rock_specular.tga");
	loadShaders("./shaders/normalMappedVertex.glsl", "./shaders/normalMappedFragment.glsl", &m_ProgramID);

	m_ambientLight = vec3(0.1f);

	return true;
}

bool AdvancedTexturing::update()
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
	
	unsigned int ambientUniform = glGetUniformLocation(m_ProgramID, "ambientLight");
	unsigned int lightDirUniform = glGetUniformLocation(m_ProgramID, "lightDir");
	unsigned int lightColorUniform = glGetUniformLocation(m_ProgramID, "lightColor");
	unsigned int specularPowerUniform = glGetUniformLocation(m_ProgramID, "specularPower");
	unsigned int eyePosUniform = glGetUniformLocation(m_ProgramID, "eyePos");

	glUniform3fv(ambientUniform, 1, (float*)&m_ambientLight);
	glUniform3fv(lightDirUniform, 1, (float*)&m_lightDir);
	glUniform3fv(lightColorUniform, 1, (float*)&m_lightColor);
	glUniform1f(specularPowerUniform, m_specularPower);

	vec3 cameraPos = m_Camera->getWorldTransform()[3].xyz;
	glUniform3fv(eyePosUniform, 1, (float*)&cameraPos);


	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? m_White : m_Black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? m_White : m_Black);
	}

	m_lightDir = (glm::rotate(dt, vec3(0, 1, 0)) * vec4(m_lightDir, 0)).xyz;

	m_Camera->update(dt);


	return true;
}

void AdvancedTexturing::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DiffuseTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NormalTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_SpecularTexture);

	int diffuseLocation = glGetUniformLocation(m_ProgramID, "diffuseTex");
	int normalLocation = glGetUniformLocation(m_ProgramID, "normalTex");
	int specularLocation = glGetUniformLocation(m_ProgramID, "specularTex");
	
	Gizmos::draw(m_Camera->getProjectionView());
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void AdvancedTexturing::shutdown()
{
	Gizmos::destroy();

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

	unsigned int index_data[6] = { 0, 1, 2, 0, 2, 3, };
	m_Quad.m_IndexCount = 6;


	//Creating the buffers
	glGenBuffers(1, &m_Quad.m_VBO); //vertex buffer object
	glGenBuffers(1, &m_Quad.m_IBO);//index buffer object
	glGenVertexArrays(1, &m_Quad.m_VAO); //vertex array object

	glBindVertexArray(m_Quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TextureVertex)* 4, vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Quad.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); // normal
	glEnableVertexAttribArray(2); // tangent
	glEnableVertexAttribArray(3); // tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(nVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(nVertex), (void*)(sizeof(vec4)));
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
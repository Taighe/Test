#include "ShadowMapping.h"

#include "FlyCamera.h"
#include "Gizmos.h"
#include "Utility.h"

ShadowMapApp::ShadowMapApp()
{
	m_Camera = new FlyCamera();
}

bool ShadowMapApp::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	Gizmos::create();

	glEnable(GL_DEPTH_TEST);

	m_backgroundColor = vec4(0.3, 0.3, 0.3, 1.0f);
	m_lightColor = vec3(1, 1, 1);
	m_GizmoSize = 10;

	m_timer = 0;
		
	//glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	loadShaders("./shaders/shadowgenVertex.glsl", "./shaders/shadowgenFragment.glsl", &m_shadowGen);
	loadShaders("./shaders/shadowmapVertex.glsl", "./shaders/shadowmapFragment.glsl", &m_program);
	generateFrameBuffer();
	generateQuad(10);

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, "./models/buddha.obj");

	if (err.size() != 0)
	{
		return false;
	}
		
	createOpenGLBuffers(shapes);

	m_lightDir = glm::normalize(glm::vec3(1, 2.5f, 1));
	mat4 lightProjection = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
	mat4 lightView = glm::lookAt(m_lightDir, vec3(0), vec3(0, 1, 0));

	m_lightMatrix = lightProjection * lightView;

	m_ambientLight = vec3(0.1, 0.1, 0.1);
	m_lightColor = vec3(1, 1, 1);
	m_materialColor = vec3(1, 1, 1);
	m_specularPower = 1;

	return true;
}

bool ShadowMapApp::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = glfwGetTime();
	m_timer += dt;
	glfwSetTime(0.0f);

	Gizmos::clear();
	vec4 m_White = vec4(1);
	vec4 m_Black = vec4(0, 0, 0, 1);

	for (int i = 0; i <= m_GizmoSize; i++)
	{
		Gizmos::addLine(vec3(-m_GizmoSize / 2 + i, -1, -m_GizmoSize / 2), vec3(-m_GizmoSize / 2 + i, -1, m_GizmoSize / 2), i == m_GizmoSize / 2 ? m_White : m_Black);
		Gizmos::addLine(vec3(-m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), vec3(m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), i == m_GizmoSize / 2 ? m_White : m_Black);
	}

	m_Camera->update(dt);

	return true;
}

void ShadowMapApp::draw()
{
	//pass shadowmap
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_shadowGen);

	int loc = glGetUniformLocation(m_shadowGen, "lightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m_lightMatrix[0][0]);

	for (unsigned int meshIndex = 0; meshIndex < m_gl_data.size(); ++meshIndex)
	{
		glBindVertexArray(m_gl_data[meshIndex].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_data[meshIndex].m_IndexCount, GL_UNSIGNED_INT, 0);
	}
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1);
	glViewport(0, 0, 1280, 720);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_program);

	loc = glGetUniformLocation(m_program, "projectionView");
	glUniformMatrix4fv(loc, 1, false, &m_Camera->getProjectionView()[0][0]);


	// bind the light matrix
	glm::mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);
	
	glm::mat4 lightMatrix = textureSpaceOffset * m_lightMatrix;
	
	loc = glGetUniformLocation(m_program, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &lightMatrix[0][0]);
	
	loc = glGetUniformLocation(m_program, "lightDir");
	glUniform3fv(loc, 1, &m_lightDir[0]);
	
	loc = glGetUniformLocation(m_program, "shadowMap");
	glUniform1i(loc, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	for (unsigned int meshIndex = 0; meshIndex < m_gl_data.size(); ++meshIndex)
	{
		glBindVertexArray(m_gl_data[meshIndex].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_data[meshIndex].m_IndexCount, GL_UNSIGNED_INT, 0);
	}
	
	glBindVertexArray(m_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	Gizmos::draw(m_Camera->getProjectionView());

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void ShadowMapApp::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

void ShadowMapApp::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* floatCount, vertexData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_data[shapeIndex].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[shapeIndex].mesh.indices.size() * sizeof(unsigned int), shapes[shapeIndex].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)* shapes[shapeIndex].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	}
}

void ShadowMapApp::generateQuad(float a_Size)
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
	m_quad.m_IndexCount = 6;

	//Creating the buffers
	glGenBuffers(1, &m_quad.m_VBO); //vertex buffer object
	glGenBuffers(1, &m_quad.m_IBO);//index buffer object
	glGenVertexArrays(1, &m_quad.m_VAO); //vertex array object

	glBindVertexArray(m_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(nVertex) * 4, vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, index_data, GL_STATIC_DRAW);

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

void ShadowMapApp::generateFrameBuffer()
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_2D, m_depth);

	// texture uses a 16-bit depth component format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// attached as a depth attachment to capture depth not colour
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth, 0);
	
	// no colour targets are used
	glDrawBuffer(GL_NONE);
	
	//Check for errors
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Error!\n");
	}
		
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
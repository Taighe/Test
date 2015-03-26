#include "Deferred.h"

#include "FlyCamera.h"
#include "Gizmos.h"
#include "Utility.h"
#include <cstdlib>

DeferredApp::DeferredApp()
{
	m_Camera = new FlyCamera();
}

bool DeferredApp::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	srand(time(nullptr));
	Gizmos::create();

	glEnable(GL_DEPTH_TEST);

	m_backgroundColor = vec4(0.3, 0.3, 0.3, 1.0f);
	m_lightColor = vec3(1, 1, 1);
	m_GizmoSize = 10;

	m_timer = 0;
	
	loadShaders("./shaders/gBufferVertex.glsl", "./shaders/gBufferFragment.glsl", &m_gPassShader);
	loadShaders("./shaders/compositeVertex.glsl", "./shaders/compositeFragment.glsl", &m_compositeShader);
	
	//direction light shaders
	loadShaders("./shaders/compositeVertex.glsl", "./shaders/dirLightFragment.glsl", &m_dirLightShader);

	//point light shaders
	loadShaders("./shaders/pointLight.vert", "./shaders/pointLight.frag", &m_pointLightShader);
	
	generateGpassFrameBuffer();
	generateLightFrameBuffer();
	generateQuad();
	generateCube();

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, "./models/buddha.obj");

	if (err.size() != 0)
	{
		return false;
	}

	createOpenGLBuffers(shapes);

	m_ambientLight = vec3(0.1, 0.1, 0.1);
	m_lightDir = vec3(0, -1, 0);
	m_lightColor = vec3(1, 1, 1);
	m_materialColor = vec3(1, 1, 1);
	m_specularPower = 1;

	return true;
}

bool DeferredApp::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	Gizmos::clear();
	vec4 m_White = vec4(1);
	vec4 m_Black = vec4(0, 0, 0, 1);

	for (int i = 0; i <= m_GizmoSize; i++)
	{
		Gizmos::addLine(vec3(-m_GizmoSize / 2 + i, -1, -m_GizmoSize / 2), vec3(-m_GizmoSize / 2 + i, -1, m_GizmoSize / 2), i == m_GizmoSize / 2 ? m_White : m_Black);
		Gizmos::addLine(vec3(-m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), vec3(m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), i == m_GizmoSize / 2 ? m_White : m_Black);
	}

	float dt = glfwGetTime();
	m_timer += dt;
	glfwSetTime(0.0f);
	m_Camera->update(dt);

	return true;
}

void DeferredApp::draw()
{
	// G-Pass: render out the albedo, position and normal
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gFbo);
	
	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	vec4 clearNormal = vec4(0.5f, 0.5f, 0.5f, 0.5f);
	vec4 clearColor = vec4(0, 0, 0, 0);

	glClearBufferfv(GL_COLOR, 0, (float*)&clearColor);
	glClearBufferfv(GL_COLOR, 1, (float*)&clearColor);
	glClearBufferfv(GL_COLOR, 2, (float*)&clearNormal);

	glUseProgram(m_gPassShader);
	// bind camera transforms
	int loc = glGetUniformLocation(m_gPassShader, "projectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_Camera->getProjectionView()[0][0]));
	
	loc = glGetUniformLocation(m_gPassShader, "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_Camera->getView()[0][0]));
	
	// draw our scene
	for (unsigned int meshIndex = 0; meshIndex < m_gl_data.size(); ++meshIndex)
	{
		glBindVertexArray(m_gl_data[meshIndex].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_data[meshIndex].m_IndexCount, GL_UNSIGNED_INT, 0);
	}
	//Gizmos::draw(m_Camera->getProjection(), m_Camera->getView());

	// Light Pass: render lights as geometry, sampling position and
	// normals disable depth testing and enable additive blending
	glBindFramebuffer(GL_FRAMEBUFFER, m_lFbo);
	
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	
	//Setting up dir light//
	
	glUseProgram(m_dirLightShader);
	
	loc = glGetUniformLocation(m_dirLightShader, "positionTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_positionTex);

	loc = glGetUniformLocation(m_dirLightShader, "normalTexture");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalTex);
	
	drawDirectionalLight(vec3(-1), vec3(1));
	
	//Setting up point lights//

	glUseProgram(m_pointLightShader);

	loc = glGetUniformLocation(m_pointLightShader, "projectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_Camera->getProjectionView()[0][0]));

	loc = glGetUniformLocation(m_pointLightShader, "posTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_positionTex);

	loc = glGetUniformLocation(m_pointLightShader, "normTexture");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalTex);

	glCullFace(GL_FRONT);
	
	//draw lights
	for (int i = 0; i < 5; i++)
	{		
		vec3 color = vec3(1, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
		drawPointLight(vec3(sinf(m_timer) * 5 + i, 3, cosf(m_timer) * 5 + i), 10, color);
	}
	

	//

	glDisable(GL_BLEND);

	glCullFace(GL_BACK);

	// Composite Pass: render a quad and combine albedo and light
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_compositeShader);
	loc = glGetUniformLocation(m_compositeShader, "albedoTexture");
	glUniform1i(loc, 0);	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedoTex);

	loc = glGetUniformLocation(m_compositeShader, "lightTexture");	
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_lTex);

	glBindVertexArray(m_quad.m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
		
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void DeferredApp::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

void DeferredApp::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
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

void DeferredApp::generateGpassFrameBuffer()
{
	// setup gpass framebuffer
	glGenFramebuffers(1, &m_gFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gFbo);
	
	glGenTextures(1, &m_albedoTex);
	glBindTexture(GL_TEXTURE_2D, m_albedoTex);
	
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glGenTextures(1, &m_positionTex);
	glBindTexture(GL_TEXTURE_2D, m_positionTex);
	
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glGenTextures(1, &m_normalTex);
	glBindTexture(GL_TEXTURE_2D, m_normalTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glGenRenderbuffers(1, &m_gDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gDepth);
	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedoTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_positionTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normalTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gDepth);
	
	GLenum gpassTargets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	
	glDrawBuffers(3, gpassTargets);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredApp::generateLightFrameBuffer()
{
	// setup light framebuffer
	glGenFramebuffers(1, &m_lFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lFbo);
	glGenTextures(1, &m_lTex);
	glBindTexture(GL_TEXTURE_2D, m_lTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_lTex, 0);
	
	GLenum lightTargets[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, lightTargets);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredApp::drawDirectionalLight(const vec3 &a_direction, const vec3 &a_diffuse)
{
	vec4 viewSpaceLight = m_Camera->getView() * vec4(glm::normalize(a_direction), 0);
	
	int loc = glGetUniformLocation(m_dirLightShader, "lightDirection");
	glUniform3fv(loc, 1, &viewSpaceLight[0]);
	
	loc = glGetUniformLocation(m_dirLightShader, "lightDiffuse");
	glUniform3fv(loc, 1, &a_diffuse[0]);
	
	glBindVertexArray(m_quad.m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DeferredApp::drawPointLight(vec3& a_position, float a_radius, vec3& a_diffuse)
{
	vec4 viewSpacePos = m_Camera->getView() * vec4(a_position, 1);

	//Opengl uniforms
	int loc = glGetUniformLocation(m_pointLightShader, "lightPosition");
	glUniform3fv(loc, 1, &a_position[0]);

	loc = glGetUniformLocation(m_pointLightShader, "lightPosView");
	glUniform3fv(loc, 1, &viewSpacePos[0]);
	
	loc = glGetUniformLocation(m_pointLightShader, "lightRadius");
	glUniform1f(loc, a_radius);

	loc = glGetUniformLocation(m_pointLightShader, "lightDiffuse");
	glUniform3fv(loc, 1, &a_diffuse[0]);

	glBindVertexArray(m_cube.m_VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

void DeferredApp::generateQuad()
{
	vec2 halfTexel = 1.0f / vec2(1280, 720);

	float vertexData[] =
	{
		-1, -1, 0, 1,	halfTexel.x, halfTexel.y,
		1, 1, 0, 1,		1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1,	halfTexel.x, 1 - halfTexel.y,
		-1, -1, 0, 1,	halfTexel.x, halfTexel.y,
		1, -1, 0, 1,	1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1,		1 - halfTexel.x, 1 - halfTexel.y,

	};

	glGenVertexArrays(1, &m_quad.m_VAO); //vertex array buffer
	glBindVertexArray(m_quad.m_VAO);

	glGenBuffers(1, &m_quad.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 6, vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DeferredApp::generateCube()
{
	float cubeVertexData[] = 
	{
		-1, -1, 1, 1,
		1, -1, 1, 1,
		1, -1, -1, 1,
		-1, -1, -1, 1,

		-1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, -1, 1,
		-1, 1, -1, 1,
	};

	unsigned int cubeIndexData[] =
	{
		0, 5, 4,
		0, 1, 5,
		1, 6, 5,
		1, 2, 6,
		2, 7, 6,
		2, 3, 7,
		3, 4, 7,
		3, 0, 4,
		4, 6, 7,
		4, 5, 6,
		3, 1, 0,
		3, 2, 1
	};

	glGenVertexArrays(1, &m_cube.m_VAO); //vertex array object
	glGenBuffers(1, &m_cube.m_VBO); //vertex buffer object
	glGenBuffers(1, &m_cube.m_IBO);//index buffer object
	
	glBindVertexArray(m_cube.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_cube.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 32, cubeVertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cube.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, cubeIndexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 36, ((char*)0) + 16);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}
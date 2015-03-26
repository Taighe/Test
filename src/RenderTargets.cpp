#include "RenderTargets.h"

#include "FlyCamera.h"
#include "Gizmos.h"
#include "Utility.h"


RenderTargets::RenderTargets()
{
	m_Camera = new FlyCamera();
}

bool RenderTargets::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	Gizmos::create();

	glEnable(GL_DEPTH_TEST);

	m_backgroundColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	m_GizmoSize = 10;

	m_timer = 0;
	
	loadShaders("./shaders/textureV_shader.glsl", "./shaders/textureF_shader.glsl", &m_program);
	generatePlane();
	generateFrameBuffer();
	
	
	return true;
}

bool RenderTargets::update()
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

	Gizmos::addSphere(vec3(0, 5, 0), 0.5f, 12, 12, vec4(1, 1, 0, 1));

	for (int i = 0; i <= m_GizmoSize; i++)
	{
		Gizmos::addLine(vec3(-m_GizmoSize / 2 + i, -1, -m_GizmoSize / 2), vec3(-m_GizmoSize / 2 + i, -1, m_GizmoSize / 2), i == m_GizmoSize / 2 ? m_White : m_Black);
		Gizmos::addLine(vec3(-m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), vec3(m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), i == m_GizmoSize / 2 ? m_White : m_Black);
	}

	m_Camera->update(dt);

	return true;
}

void RenderTargets::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 512, 512);
	
	glClearColor(1, 0, 0.75f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 world = m_Camera->getWorldTransform();
	
	vec4 plane = vec4(0, 0, 1, -5);

	vec3 reflect = glm::reflect(-world[2].xyz(), plane.xyz());
	reflect = glm::normalize(reflect);

	float dist = (glm::dot((plane.xyz() * plane.w) - world[3].xyz(), plane.xyz())) / (glm::dot(plane.xyz(), world[2].xyz()));

	vec3 inter = world[3].xyz() + (-world[2].xyz() * dist);
	vec3 up = vec3(0, 1, 0);
	world[3].xyz = inter - reflect * dist;
	 
	world[2].xyz = -reflect;
	world[0].xyz = glm::normalize(glm::cross(world[2].xyz(), up));
	world[1].xyz = glm::normalize(glm::cross(world[0].xyz(), world[2].xyz()));

	mat4 view = glm::inverse(world);
	mat4 projView = m_Camera->getProjection() * view;

	Gizmos::draw(projView);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 1280, 720);
	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::draw(m_Camera->getProjectionView());
	glUseProgram(m_program);

	int projViewHandle = glGetUniformLocation(m_program, "projectionView");
	
	glUniformMatrix4fv(projViewHandle, 1, false, &(m_Camera->getProjectionView()[0][0]));

	int projViewReflected = glGetUniformLocation(m_program, "projectionViewReflected");

	glUniformMatrix4fv(projViewReflected, 1, false, (float*)&projView);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glUniform1i(glGetUniformLocation(m_program, "diffuse"), 0);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void RenderTargets::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

void RenderTargets::generateFrameBuffer()
{
	// setup and bind framebuffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//texture setup
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0); // attach fbo as the first color attachment


	//depth setup
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	//while FBO is still bound
		
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Frame Buffer Error!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind framebuffer

}

void RenderTargets::generatePlane()
{
	float vertexData[] = {
		-5, 0, -5, 1, 0, 0,
		5, 0, -5, 1, 1, 0,
		5, 10, -5, 1, 1, 1,
		-5, 10, -5, 1, 0, 1,
	};
	
	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};
	
	m_plane.m_IndexCount = 6;

	glGenVertexArrays(1, &m_plane.m_VAO);
	glBindVertexArray(m_plane.m_VAO);

	glGenBuffers(1, &m_plane.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_plane.m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
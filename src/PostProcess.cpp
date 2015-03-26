#include "PostProcess.h"

#include "FlyCamera.h"
#include "Gizmos.h"
#include "Utility.h"


PostProcessApp::PostProcessApp()
{
	m_Camera = new FlyCamera();
}

bool PostProcessApp::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	Gizmos::create();

	glEnable(GL_DEPTH_TEST);

	m_backgroundColor = vec4(0.3, 0.3, 0.3, 1.0f);
	m_GizmoSize = 10;

	m_timer = 0;

	//create the framebuffer
	generateFrameBuffer();
	
	//create the quad mesh
	generateQuad();

	//load the post effect shader
	loadShaders("./shaders/textureV_shader.glsl", "./shaders/ppFragment.glsl", &m_program);

	return true;
}

bool PostProcessApp::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = glfwGetTime();
	m_timer += 10 * dt;
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

void PostProcessApp::draw()
{
	//bind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1280, 720);

	//render everything like normal, but to the fbo_texture

	//bind backbuffer

	//the fbo_texture

	//render our quad with the texture on it

	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Gizmos::addSphere(vec3(0,0,0), 2, 10, 10, vec4(1, 0, 0, 1));
	
	Gizmos::draw(m_Camera->getProjectionView());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	
	int uniTimer = glGetUniformLocation(m_program, "timer");
	glUniform1f(uniTimer, m_timer);

	int loc = glGetUniformLocation(m_program, "target");
	glUniform1i(loc, 0);
	
	glBindVertexArray(m_quad.m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void PostProcessApp::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

void PostProcessApp::generateFrameBuffer()
{
	glGenFramebuffers(1, &m_fbo); //generate framebuffer object
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); //bind buffer data

	glGenTextures(1, &m_tex); //generate texture object
	glBindTexture(GL_TEXTURE_2D, m_tex); //bind texture object

	glGenRenderbuffers(1, &m_depth); //generate depth
	glBindRenderbuffer(GL_RENDERBUFFER, m_depth); //bind depth

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tex, 0);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);

	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void PostProcessApp::generateQuad()
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
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
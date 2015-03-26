#include "GPUEmitter.h"
#include "gl_core_4_4.h"
#include "Utility.h"

GPUEmitter::GPUEmitter()
{
	m_particles = nullptr;
	m_maxParticles = 0;
	m_position = vec3(0, 0, 0);
	m_drawShader = 0;
	m_updateShader = 0;
	m_lastDrawTime = 0;
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}

GPUEmitter::~GPUEmitter()
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	glDeleteProgram(m_drawShader);
	glDeleteProgram(m_updateShader);
}

void GPUEmitter::Init(unsigned int a_maxParticles, float a_lifetimeMin, float a_lifetimeMax, float a_velocityMin, float a_velocityMax, float a_startSize, float a_endSize, const vec4& a_startColor, const vec4& a_endColor)
{
	m_startColor = a_startColor;
	m_endColor = a_endColor;
	m_startSize = a_startSize;
	m_endSize = a_endSize;
	m_velocityMin = a_velocityMin;
	m_velocityMax = a_velocityMax;
	m_lifespanMin = a_lifetimeMin;
	m_lifespanMax = a_lifetimeMax;
	m_maxParticles = a_maxParticles;

	m_particles = new GPUParticle[m_maxParticles];
	m_activeBuffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void GPUEmitter::CreateBuffers()
{
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);
	
	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velcocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	//setup second buffer
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUEmitter::CreateDrawShader()
{
	unsigned int vs = loadShader("./shaders/gpuParticleVert.glsl", GL_VERTEX_SHADER);
	unsigned int fs = loadShader("./shaders/gpuParticleFrag.glsl", GL_FRAGMENT_SHADER);
	unsigned int gs = loadShader("./shaders/gpuParticleGeom.glsl", GL_GEOMETRY_SHADER);

	m_drawShader = glCreateProgram();

	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, fs);
	glAttachShader(m_drawShader, gs);
	glLinkProgram(m_drawShader);

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
	glUseProgram(m_drawShader);

	// bind size information
	int location = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(location, m_startSize);
	location = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(location, m_endSize);

	// bind color information
	location = glGetUniformLocation(m_drawShader, "colorStart");
	glUniform4fv(location, 1, &m_startColor[0]);
	location = glGetUniformLocation(m_drawShader, "colorEnd");
	glUniform4fv(location, 1, &m_endColor[0]);
}

void GPUEmitter::CreateUpdateShader()
{
	unsigned int vs = loadShader("./shaders/gpuParticleUpdate.glsl", GL_VERTEX_SHADER);
	m_updateShader = glCreateProgram();
	
	glAttachShader(m_updateShader, vs);

	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_updateShader);

	glDeleteProgram(vs);

	glUseProgram(m_updateShader);

	int location = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(location, m_lifespanMin);
	location = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(location, m_lifespanMax);
}

void GPUEmitter::Draw(float a_time, const mat4& a_cameraTransform, const mat4& a_projectionView)
{
	glUseProgram(m_updateShader);

	int location = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(location, a_time);

	float deltaTime = a_time - m_lastDrawTime;
	m_lastDrawTime = a_time;
	location = glGetUniformLocation(m_updateShader, "deltaTime");
	glUniform1f(location, deltaTime);

	location = glGetUniformLocation(m_updateShader, "emitterPosition");
	glUniform3fv(location, 1, &m_position[0]);

	//disable rasterisation
	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vao[m_activeBuffer]);

	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	//disable transform feedback and enable rasterization again
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	//draw the particles using the Geometry shader to billboard them
	glUseProgram(m_drawShader);

	location = glGetUniformLocation(m_drawShader, "projectionView");
	glUniformMatrix4fv(location, 1, false, &a_projectionView[0][0]);

	location = glGetUniformLocation(m_drawShader, "cameraTransform");
	glUniformMatrix4fv(location, 1, false, &a_cameraTransform[0][0]);

	glBindVertexArray(m_vao[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);
	
	m_activeBuffer = otherBuffer;
}
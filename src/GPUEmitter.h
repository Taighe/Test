#ifndef _GPUEMIT_H
#define _GPUEMIT_H

#include "glm_header.h"

struct GPUParticle
{
	GPUParticle() : lifetime(1), lifespan(0) {}
	vec3 position;
	vec3 velocity;
	float lifetime;
	float lifespan;
};

class GPUEmitter
{
public:
	GPUEmitter();
	virtual ~GPUEmitter();
	void Init(unsigned int a_maxParticles, float a_lifetimeMin, float a_lifetimeMax, float a_velocityMin,
		float a_velocityMax, float a_startSize, float a_endSize, const vec4& a_startColor, const vec4& a_endColor);

	void Draw(float a_time, const mat4& a_cameraTransform, const mat4& a_projectionView);
	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();

	GPUParticle* m_particles;

	vec4 m_startColor;
	vec4 m_endColor;
	vec3 m_position;

	unsigned int m_firstDead;
	unsigned int m_maxParticles;

	float m_lifespanMin;
	float m_lifespanMax;
	float m_velocityMin;
	float m_velocityMax;
	float m_startSize;
	float m_endSize;

	unsigned int m_activeBuffer;
	unsigned int m_vao[2];
	unsigned int m_vbo[2];
	unsigned int m_drawShader;
	unsigned int m_updateShader;

	float m_lastDrawTime;

};

#endif
#ifndef _PARTICLEEMIT_H
#define _PARTICLEEMIT_H

#include "glm_header.h"

struct Particle
{
	vec3 position;
	vec3 velocity;
	vec4 color;
	float size;
	float lifetime;
	float lifespan;
};

struct ParticleVertex
{
	vec4 position;
	vec4 color;
};

class ParticleEmitter
{
public:
	ParticleEmitter();
	void Init(unsigned int a_maxParticles, unsigned int a_emitRate, float a_lifetimeMin, float a_lifetimeMax, float a_velocityMin, 
				float a_velocityMax, float a_startSize, float a_endSize, const vec4& a_startColor, const vec4& a_endColor);
	void Emit();
	void Draw();
	void Update(float a_deltaTime, const mat4& a_cameraTransform);
	virtual ~ParticleEmitter();

	Particle* m_particles;

	ParticleVertex* m_vertexData;
	vec4 m_startColor;
	vec4 m_endColor;
	vec3 m_position;
	vec3 m_direction;
	vec3 m_point2;
	unsigned int m_firstDead;
	unsigned int m_maxParticles;
	unsigned int m_vao, m_vbo, m_ibo;

	float m_gravity;
	float m_emitTimer;
	float m_emitRate;
	float m_lifespanMin;
	float m_lifespanMax;
	float m_velocityMin;
	float m_velocityMax;
	float m_startSize;
	float m_endSize;


};

#endif
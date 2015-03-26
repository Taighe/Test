#ifndef _SHADOWMAP_H
#define _SHADOWMAP_H

#include "baseApp.h"
#include "Vertex.h"
#include "tiny_obj_loader.h"

class Camera;

class ShadowMapApp : public Application
{
public:
	ShadowMapApp();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

	void generateFrameBuffer();
	void generateQuad(float a_Size);
	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);

private:
	Camera* m_Camera;
	std::vector<OpenGlData> m_gl_data;
	int m_GizmoSize;
	vec4 m_backgroundColor;
	float m_timer;
	
	mat4 m_lightMatrix;
	vec3 m_lightDir;
	vec3 m_lightColor;
	vec3 m_materialColor;
	vec3 m_ambientLight;
	float m_specularPower;

	//FrameBuffer Object
	unsigned int m_fbo;
	//Color texture
	unsigned int m_tex;
	//Depth Texture
	unsigned int m_depth;
	//quad
	OpenGlData m_quad;

	//post process program
	unsigned int m_program;
	unsigned int m_shadowGen;
};

#endif
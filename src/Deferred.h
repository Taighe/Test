#ifndef _DEFERRED_H
#define _DEFERRED_H

#include "baseApp.h"
#include "Vertex.h"
#include "tiny_obj_loader.h"

class Camera;

class DeferredApp : public Application
{
public:
	DeferredApp();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

	void generateGpassFrameBuffer();
	void generateLightFrameBuffer();
	void generateQuad();
	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	void drawDirectionalLight(const vec3 &a_direction, const vec3 &a_diffuse);
	void drawPointLight(vec3& a_position, float a_radius, vec3& a_diffuse);
	void generateCube();

private:
	Camera* m_Camera;
	std::vector<OpenGlData> m_gl_data;
	int m_GizmoSize;
	vec4 m_backgroundColor;
	float m_timer;

	vec3 m_lightDir;
	vec3 m_lightColor;
	vec3 m_materialColor;
	vec3 m_ambientLight;
	float m_specularPower;

	//Gpass FrameBuffer Object
	unsigned int m_gFbo;
	//Color texture
	unsigned int m_albedoTex;
	unsigned int m_positionTex;
	unsigned int m_normalTex;

	//Depth Texture
	unsigned int m_gDepth;
	
	//Light FrameBuffer Object
	unsigned int m_lFbo;
	//Color texture
	unsigned int m_lTex;

	//quad
	OpenGlData m_quad;
	OpenGlData m_cube;

	//shader programs
	unsigned int m_gPassShader;
	unsigned int m_dirLightShader;
	unsigned int m_compositeShader;
	unsigned int m_pointLightShader;
};

#endif
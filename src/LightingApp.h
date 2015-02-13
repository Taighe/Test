#ifndef _LIGHTING_APP_H
#define _LIGHTING_APP_H

#include "baseApp.h"
#include "tiny_obj_loader.h"
#include "glm_header.h"
#include "Vertex.h"

class Camera;

class LightingApp : public Application
{
public:
	LightingApp();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	void cleanupOpenGLBuffers();
	void reloadShader();

private:
	Camera* m_Camera;
	std::vector<OpenGlData> m_gl_data;	
	vec3 m_lightDir;
	vec3 m_lightColor;
	vec3 m_materialColor;
	vec3 m_ambientLight;
	float m_specularPower;
	float m_Timer;
	unsigned int m_ProgramID;
	unsigned int m_IndexCount;
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

};

#endif
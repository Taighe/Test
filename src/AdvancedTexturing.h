#ifndef _ADVANCEDTEX_H
#define _ADVANCEDTEX_H

#include "baseApp.h"
#include "glm_header.h"
#include "Vertex.h"
#include "AntTweakBar.h"

class Camera;

class AdvancedTexturing : public Application
{
public:
	AdvancedTexturing();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();
	void generateQuad(float a_Size);
	void loadTexture(char* a_DiffuseTexture, char* a_NormalTexture, char* a_SpecularTexture);

private:
	Camera* m_Camera;
	OpenGlData m_Quad;
	mat4 m_Projection;
	mat4 m_View;
	vec3 m_CameraPos;	
	vec3 m_lightDir;
	vec3 m_lightColor;
	vec3 m_ambientLight;
	vec3 m_eyePos;
	int m_GizmoSize;
	float m_FPS;
	vec4 m_backgroundColor;
	float m_specularPower;
	unsigned int m_ProgramID;
	unsigned int m_DiffuseTexture; 
	unsigned int m_NormalTexture;
	unsigned int m_SpecularTexture;
	unsigned int m_NormalMap;
	TwBar* m_Bar;
};

#endif
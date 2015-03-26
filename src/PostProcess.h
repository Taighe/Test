#ifndef _POSTPROCESS_H
#define _POSTPROCESS_H

#include "baseApp.h"
#include "Vertex.h"

class Camera;

class PostProcessApp : public Application
{
public:
	PostProcessApp();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

	void generateFrameBuffer();
	void generateQuad();

private:
	Camera* m_Camera;
	int m_GizmoSize;
	vec4 m_backgroundColor;
	float m_timer;


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
};

#endif
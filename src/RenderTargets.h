#ifndef _RENDER_H
#define _RENDER_H

#include "baseApp.h"
#include "Vertex.h"

class Camera;


class RenderTargets : public Application
{
public:
	RenderTargets();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

	void generateFrameBuffer();
	void generatePlane();

private:
	Camera* m_Camera;
	int m_GizmoSize;
	vec4 m_backgroundColor;
	float m_timer;

	OpenGlData m_plane;
	unsigned int m_fbo, m_fboTexture, m_fboDepth;
	unsigned int m_program;
};

#endif
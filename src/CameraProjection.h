#ifndef _CAM_PROJ_H
#define _CAM_PROJ_H

#include "baseApp.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <cstdio>
#include "Gizmos.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera;

class CameraProjection : public Application
{
public:
	CameraProjection();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

private:
	mat4 m_Projection;
	mat4 m_View;
	vec3 m_CameraPos;
	Camera* m_Camera;

};

#endif
#ifndef _SOLAR_H
#define _SOLAR_H

#include "baseApp.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"


using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class SolarSystem : public Application
{

public:
	SolarSystem();

	bool startup();
	
	bool update();
	
	void draw(); 
	
	void shutdown();

	mat4 BuildOrbitMatrix(float local_rotation, float radius, float orbit_rotation);


private:
	float m_Timer;
	vec3 m_CameraPos;
	mat4 m_Projection;
	vec4 m_White;
	vec4 m_Yellow;
	vec4 m_Black;
	mat4 m_View;
	float m_Rotate;

};

#endif
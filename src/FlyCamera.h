#ifndef _FLYCAM_H
#define _FLYCAM_H

#include "baseCamera.h"

class FlyCamera : public Camera
{
public:	
	FlyCamera();

	void update(float a_DeltaTime);

	void setSpeed(float a_Speed);

private:
	bool m_EnableMouse;
	vec3 m_Direction;
	float m_Speed;
	float m_Timer;
	double m_MousePosX;
	double m_MousePosY;
};

#endif
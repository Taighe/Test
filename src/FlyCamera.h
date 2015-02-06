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
	float m_Speed;
	double m_MousePosX;
	double m_MousePosY;
};

#endif
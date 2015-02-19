#ifndef _QUAT_H
#define _QUAT_H

#include "baseApp.h"
#include "glm_header.h"

class Camera;

struct KeyFrame
{
	vec3 position;
	glm::quat rotation;
};

class Quaternion : public Application
{

public:
	Quaternion();

	bool startup();

	bool update();

	void draw();

	void shutdown();

	mat4 Quaternion::EvaluateKeyFrames(KeyFrame a_Start, KeyFrame a_End, float a_Time);



private:
	float m_Timer;
	vec3 m_CameraPos;
	mat4 m_Projection;
	vec4 m_White;
	vec4 m_Black;
	mat4 m_View;
	Camera* m_Camera;
	
	KeyFrame m_HipFrames[2];
	KeyFrame m_KneeFrames[2];
	KeyFrame m_AnkleFrames[2];

	mat4 m_HipBone;
	mat4 m_KneeBone;
	mat4 m_AnkleBone;
	vec3 m_Positions[2];
	glm::quat m_Rotations[2];
	float m_Rotate;

};

#endif
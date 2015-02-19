#include "Quaternion.h"
#include <cstdio>
#include "Gizmos.h"
#include "FlyCamera.h"


Quaternion::Quaternion()
{
	m_White = vec4(1);
	m_Black = vec4(0, 0, 0, 1);
	m_Camera = new FlyCamera();
}

bool Quaternion::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	Gizmos::create();
	glm::quat boringQuaternion(1, 0, 0 ,0);
	glm::quat eulerQuaternion(vec3(3, 5, 7));

	m_Positions[0] = vec3(10, 5, 10);
	m_Positions[1] = vec3(-10, 0, -10);
	m_Rotations[0] = glm::quat(vec3(0, -1, 0));
	m_Rotations[1] = glm::quat(vec3(0, 1, 0));

	float PI = 3.14159;

	m_HipFrames[0].position = glm::vec3(0, 5, 0);
	m_HipFrames[0].rotation = glm::quat(vec3(1, 0, 0));
	m_HipFrames[1].position = glm::vec3(0, 5, 0);
	m_HipFrames[1].rotation = glm::quat(vec3(-1, 0, 0));

	m_KneeFrames[0].position = glm::vec3(0, -2.5f, 0);
	m_KneeFrames[0].rotation = glm::quat(vec3(1, 0, 0));
	m_KneeFrames[1].position = glm::vec3(0, -2.5f, 0);
	m_KneeFrames[1].rotation = glm::quat(vec3(0, 0, 0));

	m_AnkleFrames[0].position = glm::vec3(0,-2.5f, 0);
	m_AnkleFrames[0].rotation = glm::quat(vec3(-1, 0, 0));
	m_AnkleFrames[1].position = glm::vec3(0, -2.5f, 0);
	m_AnkleFrames[1].rotation = glm::quat(vec3(0, 0, 0));



	return true;
}

bool Quaternion::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = glfwGetTime();
	glfwSetTime(0.0f);

	m_Timer += dt;

	float sinwave = sinf(m_Timer) * 0.5f + 0.5f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();

	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? m_White : m_Black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? m_White : m_Black);
	}
	
	//Gizmos::addTransform(transform);
	//Gizmos::addAABBFilled(finalPos, vec3(1), vec4(0, 0, 1, 1), &transform);

	// linearly interpolate hip position
	glm::vec3 p = (1.0f - sinwave) * m_HipFrames[0].position + sinwave * m_HipFrames[1].position;

	// spherically interpolate hip rotation
	glm::quat finalRotation = glm::slerp(m_HipFrames[0].rotation, m_HipFrames[1].rotation, sinwave);

	//update the hip bone
	m_HipBone = glm::translate(p) * glm::toMat4(finalRotation);

	// linearly interpolate knee position
	p = (1.0f - sinwave) * m_KneeFrames[0].position + sinwave * m_KneeFrames[1].position;

	// spherically interpolate knee rotation
	finalRotation = glm::slerp(m_KneeFrames[0].rotation, m_KneeFrames[1].rotation, sinwave);
	
	//update the knee bone
	m_KneeBone = m_HipBone * glm::translate(p) * glm::toMat4(finalRotation);

	// linearly interpolate ankle position
	p = (1.0f - sinwave) * m_AnkleFrames[0].position + sinwave * m_AnkleFrames[1].position;

	// spherically interpolate ankle rotation
	finalRotation = glm::slerp(m_AnkleFrames[0].rotation, m_AnkleFrames[1].rotation, sinwave);

	//update the ankle bone
	m_AnkleBone = m_KneeBone * glm::translate(p) * glm::toMat4(finalRotation);

	glm::vec3 hipPos = glm::vec3(m_HipBone[3].x,m_HipBone[3].y, m_HipBone[3].z);
	glm::vec3 kneePos = glm::vec3(m_KneeBone[3].x, m_KneeBone[3].y, m_KneeBone[3].z);
	glm::vec3 anklePos = glm::vec3(m_AnkleBone[3].x, m_AnkleBone[3].y, m_AnkleBone[3].z);
	glm::vec3 half(0.5f);
	glm::vec4 pink(1, 0, 1, 1);
	Gizmos::addAABBFilled(hipPos, half, pink, &m_HipBone);
	Gizmos::addAABBFilled(kneePos, half, pink, &m_KneeBone);
	Gizmos::addAABBFilled(anklePos, half, pink, &m_AnkleBone);
	
	m_Camera->update(dt);
	return true;
}

mat4 Quaternion::EvaluateKeyFrames(KeyFrame a_Start, KeyFrame a_End, float a_Time)
{
	return mat4(1);
}

void Quaternion::draw()
{
	Gizmos::draw(m_Camera->getProjectionView());
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Quaternion::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}
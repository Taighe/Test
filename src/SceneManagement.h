#ifndef _SCENE_H
#define _SCENE_H

#include "baseApp.h"
#include "Vertex.h"
#include <vector>

class Camera;

class AABB
{
public:
	AABB(){ reset(); }
	~AABB(){}

	void reset()
	{
		min.x = min.y = min.z = 1e37f;
		max.x = max.y = max.z = -1e37f;
	}
	
	void fit(const std::vector<vec3>& points)
	{
		for (int p = 0; p < points.size(); p++)
		{
			if (points[p].x < min.x) min.x = points[p].x;
			if (points[p].y < min.y) min.y = points[p].y;
			if (points[p].z < min.z) min.z = points[p].z;
			if (points[p].x > max.x) max.x = points[p].x;
			if (points[p].y > max.y) max.y = points[p].y;
			if (points[p].z > max.z) max.z = points[p].z;
		}
	}

	vec3 min, max;
};

class BoundingSphere
{
public:
	BoundingSphere() : centre(0), radius(0) { }
	~BoundingSphere() {}

	void fit(const std::vector<vec3>& points)
	{
		vec3 min(1e37f), max(-1e37f);

		for (int p = 0; p < points.size(); p++)
		{
			if (points[p].x < min.x) min.x = points[p].x;
			if (points[p].y < min.y) min.y = points[p].y;
			if (points[p].z < min.z) min.z = points[p].z;
			if (points[p].x > max.x) max.x = points[p].x;
			if (points[p].y > max.y) max.y = points[p].y;
			if (points[p].z > max.z) max.z = points[p].z;
		}

		centre = (min + max) * 0.5f;
		radius = distance(min, centre);
	}

	vec3 centre;
	float radius;
};

class SceneManagement : public Application
{
public:
	SceneManagement();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();
	
	void getFrustumPlanes(const mat4& transform, vec4* planes);
	void renderPlane(vec4 plane);

private:
	BoundingSphere m_sphere;
	Camera* m_Camera;
	vec4 m_backgroundColor;
	float m_timer;
	int m_GizmoSize;

};

#endif
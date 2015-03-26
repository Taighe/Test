#include "SceneManagement.h"

#include "FlyCamera.h"
#include "Gizmos.h"
#include "glm_header.h"


SceneManagement::SceneManagement()
{
	m_Camera = new FlyCamera();
}

bool SceneManagement::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	Gizmos::create();

	glEnable(GL_DEPTH_TEST);

	m_backgroundColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	m_GizmoSize = 10;

	m_timer = 0;

	m_sphere.centre = vec3(0, 0, 0);
	m_sphere.radius = 0.5f;


	return true;
}

bool SceneManagement::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = glfwGetTime();
	m_timer += dt;
	glfwSetTime(0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	vec4 m_White = vec4(1);
	vec4 m_Black = vec4(0, 0, 0, 1);

	for (int i = 0; i <= m_GizmoSize; i++)
	{
		Gizmos::addLine(vec3(-m_GizmoSize / 2 + i, -1, -m_GizmoSize / 2), vec3(-m_GizmoSize / 2 + i, -1, m_GizmoSize / 2), i == m_GizmoSize / 2 ? m_White : m_Black);
		Gizmos::addLine(vec3(-m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), vec3(m_GizmoSize / 2, -1, -m_GizmoSize / 2 + i), i == m_GizmoSize / 2 ? m_White : m_Black);
	}


	m_sphere.centre = vec3(0, 1 + cosf(m_timer), 0);

	Gizmos::addSphere(m_sphere.centre, m_sphere.radius, 8, 8, vec4(1, 0, 0, 1));
	vec4 planeColor(1, 1, 0, 1);

	m_Camera->update(dt);
	

	vec4 planes[6];
	getFrustumPlanes(m_Camera->getProjectionView(), planes);
	
	for (int i = 0; i < 6; i++)
	{
		
		float d = glm::dot(vec3(planes[i]), m_sphere.centre) + planes[i].w;
		renderPlane(planes[i]);
		if (d < -m_sphere.radius)
		{
			printf("Behind don't render.\n");
			break;
		}
		else if (d < m_sphere.radius)
		{
			
			printf("Touching.\n");
		}
		else
		{
			printf("Front fully visible so render.\n");
		}

		
	}

	Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(-4, 1, 4), planeColor);
	Gizmos::addTri(vec3(4, 1, 4), vec3(4, 1, -4), vec3(-4, 1, -4), planeColor);

	

	return true;
}

void SceneManagement::draw()
{
	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w);

	Gizmos::draw(m_Camera->getProjectionView());



	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void SceneManagement::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

void SceneManagement::getFrustumPlanes(const mat4& transform, vec4* planes)
{

	planes[0] = vec4(transform[0][3] - transform[1][0],
					transform[1][3] - transform[1][0],
					transform[2][3] - transform[2][0],
					transform[3][3] - transform[3][0]);

	planes[1] = vec4(transform[0][3] + transform[0][0],
					transform[1][3] + transform[1][0],
					transform[2][3] + transform[2][0],
					transform[3][3] + transform[3][0]);

	planes[2] = vec4(transform[0][3] - transform[0][1],
					transform[1][3] - transform[1][1],
					transform[2][3] - transform[2][1],
					transform[3][3] - transform[3][1]);

	planes[3] = vec4(transform[0][3] + transform[0][1],
					transform[1][3] + transform[1][1],
					transform[2][3] + transform[2][1],
					transform[3][3] + transform[3][1]);

	planes[4] = vec4(transform[0][3] - transform[0][2],
					transform[1][3] - transform[1][2],
					transform[2][3] - transform[2][2],
					transform[3][3] - transform[3][2]);
	
	planes[5] = vec4(transform[0][3] + transform[0][2],
					transform[1][3] + transform[1][2],
					transform[2][3] + transform[2][2],
					transform[3][3] + transform[3][2]);

	for (int i = 0; i < 6; i++)
	{
		planes[i] = glm::normalize(planes[i]);
	}
	
}

void SceneManagement::renderPlane(vec4 plane)
{
	//plane made up of [xyz]normal [d]distance
	vec3 up = vec3(0, 1, 0);
	if (plane.xyz() == vec3(0, 1, 0)){ up = vec3(1, 0, 0); }
	
	vec3 tangent = glm::normalize(glm::cross(plane.xyz(), up));
	vec3 bitangent = glm::normalize(glm::cross(plane.xyz(), tangent));

	vec3 p = plane.xyz() * plane.w;
	vec3 v0 = p + tangent + bitangent;
	vec3 v1 = p + tangent - bitangent;
	vec3 v2 = p - tangent - bitangent;
	vec3 v3 = p - tangent + bitangent;

	Gizmos::addTri(v0, v1, v2, vec4(1, 0, 1, 1));
	Gizmos::addTri(v0, v1, v3, vec4(1, 0, 1, 1));

}


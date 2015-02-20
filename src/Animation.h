#ifndef _ANIM_H
#define _ANIM_H

#include "baseApp.h"
#include "glm_header.h"
#include "FBXFile.h"
#include "Vertex.h"

class Camera;

class Animation : public Application
{
public:
	Animation();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();
	void generateGLMeshes(FBXFile* a_fbx);
	void evaluateSkeleton(FBXAnimation* a_anim, FBXSkeleton* a_skeleton, float a_timer);
	void updateBones(FBXSkeleton* a_skeleton);

private:	
	FBXFile* m_FbxFile;
	Camera* m_Camera;
	std::vector<OpenGlData> m_Meshes;
	vec4 m_BackgroundColor;
	float m_Timer;
	unsigned int m_ProgramID;
	int m_GizmoSize;

};

#endif
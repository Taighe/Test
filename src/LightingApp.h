#ifndef _LIGHTING_APP_H
#define _LIGHTING_APP_H

#include "baseApp.h"


class FBXFile;
class Camera;

class LightingApp : public Application
{
public:
	LightingApp();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

	void createOpenGLBuffers(FBXFile* a_File);
	void cleanupOpenGLBuffers(FBXFile* a_File);
	void reloadShader();

private:
	unsigned int m_ProgramID;
	Camera* m_Camera;
	FBXFile* m_File;
};

#endif
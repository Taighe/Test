//#ifndef _LIGHTING_APP_H
//#define _LIGHTING_APP_H
//
//#include "baseApp.h"
//
//
//class FBXFile;
//class Camera;
//
//class LightingApp : public Application
//{
//public:
//	LightingApp();
//	virtual bool startup();
//	virtual bool update();
//	virtual void draw();
//	virtual void shutdown();
//
//	void createOpenGLBuffers(FBXFile* a_File);
//	void cleanupOpenGLBuffers(FBXFile* a_File);
//	void reloadShader();
//
//private:
//	Camera* m_Camera;
//	FBXFile* m_File;
//	unsigned int m_ProgramID;
//	unsigned int m_IndexCount;
//	unsigned int m_VAO;
//	unsigned int m_VBO;
//	unsigned int m_IBO;
//
//};
//
//#endif
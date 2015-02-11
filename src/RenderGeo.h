#ifndef _RENDERGEO_H
#define _RENDERGEO_H


#include "baseApp.h"
#include "FlyCamera.h"

class RenderingGeometry : public Application
{
public:
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();
	void generateGrid(unsigned int a_Rows, unsigned int a_Cols);
	void generateShader();


protected:
	Camera* m_Camera;
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_ProgramID;
	unsigned int m_IndexCount;
	float m_Timer;
};

#endif
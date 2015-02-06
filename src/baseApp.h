#ifndef _APPLICATION_H
#define _APPLICATION_H

struct GLFWwindow;

class Application
{
public:
	Application();
	virtual ~Application();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

protected:
	GLFWwindow* m_Window;
};

#endif
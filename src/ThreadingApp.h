#ifndef _THREAD_H
#define _THREAD_H

#include "baseApp.h"
#include <thread>
#include <cstdio>
#include <vector>
#include <mutex>


class ThreadingApp : public Application
{
public:
	ThreadingApp();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();

	

private:
	std::vector<std::thread> m_threads;

};

#endif
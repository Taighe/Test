#include "ThreadingApp.h"


ThreadingApp::ThreadingApp()
{
	
}

void printFoo(int i)
{
	static std::mutex myMutex;
	std::lock_guard<std::mutex> guard(myMutex);
	
	printf("Hello Thread %i\n", i);
	printf("Im here\n");
	printf("...not there\n");

}

bool ThreadingApp::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	for (int i = 0; i < 50; i++)
	{
		m_threads.push_back(std::thread(printFoo, i));
	}

	for (int i = 0; i < m_threads.size(); i++)
	{
		m_threads[i].join();
	}

	return true;
}

bool ThreadingApp::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	return true;
}

void ThreadingApp::draw()
{

}

void ThreadingApp::shutdown()
{

	Application::shutdown();
}



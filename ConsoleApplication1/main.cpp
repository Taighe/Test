#include "solarSystem.h"

int main() 
{
	Application* app = new SolarSystem();
	if (app->startup() == true) 
	{
		while (app->update() == true)
		{
			app->draw();
		}
		app->shutdown();
	}
		 
	delete app;
	return 0;
}
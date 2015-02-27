#include "GPUParticleApp.h"

int main()
{
	Application* app = new GPUParticleApp();
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
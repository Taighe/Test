#include "Deferred.h"

int main()
{
	
	Application* app = new DeferredApp();
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
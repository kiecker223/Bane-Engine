#include "Application/Application.h"
#include <Windows.h>


int main(int argc, char** argv)
{
	Application App(argc, argv);
	App.InitSystems();
	if (App.HasCustomUpdateFunction())
	{
		auto CustomUpdateFunc = App.GetUpdateCallback();
		CustomUpdateFunc();
	}
	else
	{
		App.Run();
	}
	App.Shutdown();
}


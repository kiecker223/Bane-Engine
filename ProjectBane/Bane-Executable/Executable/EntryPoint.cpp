#include "Application.h"
#include <Windows.h>


int main(int argc, char** argv)
{
	BANE_CHECK(SetThreadAffinityMask(GetCurrentThread(), 1) != 0);
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


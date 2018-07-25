#include "Application.h"



int main(int argc, char** argv)
{
	Application App(argc, argv);
	App.InitSystems();
	App.Run();
	App.Shutdown();
}


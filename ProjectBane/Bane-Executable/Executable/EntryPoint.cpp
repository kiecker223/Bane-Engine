#include <Engine/Application/Application.h>


int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);
	Application App;
	App.InitSystems();
	App.Run();
	App.Shutdown();
}


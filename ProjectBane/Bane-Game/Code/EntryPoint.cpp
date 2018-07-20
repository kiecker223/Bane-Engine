#include "BaneObject/Scene/SceneManager.h"
#include <iostream>

Scene* CurrentScene;

extern "C" void InitApplication()
{
	CurrentScene = GetSceneManager()->CreateNewScene("NewScene");
	std::cout << "Application Initialized" << std::endl;
}

void UpdateApplication()
{
}

void CleanupApplication()
{
	SetCurrentScene(nullptr);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	UNUSED(lpReserved);
	UNUSED(hModule);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBox(0,"Test", "From unmanaged dll",0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
} 



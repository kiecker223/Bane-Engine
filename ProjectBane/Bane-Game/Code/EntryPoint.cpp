#include "BaneObject/Scene/SceneManager.h"
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"
#include <iostream>




void InitApplication()
{
	GetSceneManager()->CreateNewScene("NewScene");
	Scene* Test = GetSceneManager()->CurrentScene;

	Entity* EntityTest = Test->CreateEntity("Test Entity");
	auto* TestMesh = EntityTest->AddComponent<MeshRenderingComponent>();
	//TestMesh->RenderedMesh.LoadFromFile("")
	UNUSED(TestMesh);

	UNUSED(Test);
	std::cout << "Application Initialized" << std::endl;
}


void UpdateApplication()
{
}

void CleanupApplication()
{
//	SetCurrentScene(nullptr);
}

//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//                     )
//{
//	UNUSED(lpReserved);
//	UNUSED(hModule);
//
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
////        MessageBox(0,"Test", "From unmanaged dll",0);
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//} 



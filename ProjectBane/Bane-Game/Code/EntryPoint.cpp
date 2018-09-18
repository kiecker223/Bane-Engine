#include "BaneObject/Scene/SceneManager.h"
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"
#include <iostream>
#include "Core/Containers/HeapQueue.h"
#include "Graphics/IO/TextureCache.h"
#include "BaneObject/CoreComponents/CameraComponent.h"




void InitApplication()
{
	GetSceneManager()->CreateNewScene("NewScene");
	Scene* Test = GetSceneManager()->CurrentScene;

	Entity* EntityTest = Test->CreateEntity("Test Entity");
	auto* TestMesh = EntityTest->AddComponent<MeshRenderingComponent>();
	TestMesh->RenderedMesh = Test->GetMeshCache().LoadMesh("Meshes/PlaneyThing.fbx");
	TestMesh->RenderedMaterial.InitializeMaterial("TestRenderingShader.gfx");
	auto* Albedo = GetTextureCache()->LoadTexture("Resources/AbstractTex.jpg");
	TestMesh->RenderedMaterial.SetTexture(Albedo, 0);
	auto* Normal = GetTextureCache()->LoadTexture("Resources/brickwork-normal.jpg");
	TestMesh->RenderedMaterial.SetTexture(Normal, 1);
	auto TestPos = EntityTest->GetTransform()->GetPosition();
	UNUSED(TestPos);

	Entity* MainCamera = Test->CreateEntity("TestCamera");
	MainCamera->GetTransform()->SetPosition(float3(0.f, 0.f, -2.f));
	MainCamera->AddComponent<CameraComponent>();
	auto CameraTest = MainCamera->GetTransform()->GetForward();
	UNUSED(CameraTest);

	auto* Skybox = GetTextureCache()->LoadCubemap("Testskybox", "Resources/nx.png", "Resources/ny.png", "Resources/nz.png", "Resources/px.png", "Resources/py.png", "Resources/pz.png");
	Test->SetSkybox(Skybox);
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



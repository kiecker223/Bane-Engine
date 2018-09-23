#include "BaneObject/Scene/SceneManager.h"
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"
#include <iostream>
#include "Core/Containers/HeapQueue.h"
#include "Graphics/IO/TextureCache.h"
#include "BaneObject/CoreComponents/CameraComponent.h"
#include "CameraMovementComponent.h"




void InitApplication()
{
	GetSceneManager()->CreateNewScene("NewScene");
	Scene* Test = GetSceneManager()->CurrentScene;

	Entity* EntityTest = Test->CreateEntity("Test Entity");
	EntityTest->GetPhysicsProperties().Velocity = float3(0.f, 0.f, 0.f);
	EntityTest->GetPhysicsProperties().Mass = 10000.f;
	EntityTest->GetTransform()->SetScale(float3(0.1f, 0.1f, 0.1f));
	auto* TestMesh = EntityTest->AddComponent<MeshRenderingComponent>();
	TestMesh->RenderedMesh = Test->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
	TestMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
	auto* Albedo = GetTextureCache()->LoadTexture("Resources/AbstractTex.jpg");
	TestMesh->RenderedMaterial.SetTexture(Albedo, 0);
	auto* Normal = GetTextureCache()->LoadTexture("Resources/brickwork-normal.jpg");
	TestMesh->RenderedMaterial.SetTexture(Normal, 1);
	//EntityTest->AddComponent<TestComponent1>();

	MeshRenderingComponent* OtherMesh = Test->CreateEntity("Other test entity")->AddComponent<MeshRenderingComponent>();
	OtherMesh->GetOwner()->GetPhysicsProperties().Mass = 1000.f;
	OtherMesh->GetOwner()->GetTransform()->Scale(0.1f);
	OtherMesh->RenderedMesh = Test->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
	OtherMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
	OtherMesh->GetTransform()->SetPosition(float3(1.f, 2.f, 3.f));


	MeshRenderingComponent* OtherMesh2 = Test->CreateEntity("Other test entity 2")->AddComponent<MeshRenderingComponent>();
	OtherMesh2->GetOwner()->GetTransform()->Scale(0.1f);
	OtherMesh2->GetOwner()->GetPhysicsProperties().Mass = 1000.f;
	OtherMesh2->RenderedMesh = Test->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
	OtherMesh2->RenderedMaterial.InitializeMaterial("MainShader.gfx");
	OtherMesh2->GetTransform()->SetPosition(float3(1.f, 2.f, -3.f));


	Entity* MainCamera = Test->CreateEntity("TestCamera");
	MainCamera->GetPhysicsProperties().bCanTick = false;
	MainCamera->GetTransform()->SetPosition(float3(0.f, 0.f, -12.f));
	MainCamera->AddComponent<CameraComponent>();
	MainCamera->AddComponent<CameraMovementComponent>();

	auto* Skybox = GetTextureCache()->LoadCubemap("Testskybox", "Resources/nx.png", "Resources/ny.png", "Resources/nz.png", "Resources/px.png", "Resources/py.png", "Resources/pz.png");
	Test->SetSkybox(Skybox);
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



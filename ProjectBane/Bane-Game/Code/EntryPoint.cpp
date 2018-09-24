#include "BaneObject/Scene/SceneManager.h"
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"
#include <iostream>
#include "Core/Containers/HeapQueue.h"
#include "Graphics/IO/TextureCache.h"
#include "BaneObject/CoreComponents/CameraComponent.h"
#include "CameraMovementComponent.h"


class CameraFollowPlanet : public Component 
{
	IMPLEMENT_COMPONENT(CameraFollowPlanet)
public:

	Entity* ParentPlanet;
	CameraComponent* CamComp;

	void Start()
	{
		CamComp = GetOwner()->GetComponent<CameraComponent>();
	}

	void Tick(float Dt) override
	{
		UNUSED(Dt);
		float3 PlanetPos = ParentPlanet->GetTransform()->GetPosition();
		CamComp->Target = PlanetPos;
		GetTransform()->Translate((PlanetPos - GetTransform()->GetPosition()) / 1000.f);
	}

};

void InitApplication2()
{
	GetSceneManager()->CreateNewScene("NewScene");
	Scene* Test = GetSceneManager()->CurrentScene;

	Entity* EntityTest = Test->CreateEntity("Test Entity");
	EntityTest->GetPhysicsProperties().Mass = 100000.;
	EntityTest->GetTransform()->SetScale(float3(0.1f, 0.1f, 0.1f));
	EntityTest->GetTransform()->SetPosition(float3(0.f, 0.f, 0.f));
	auto* TestMesh = EntityTest->AddComponent<MeshRenderingComponent>();
	TestMesh->RenderedMesh = Test->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
	TestMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
	auto* Albedo = GetTextureCache()->LoadTexture("Resources/AbstractTex.jpg");
	TestMesh->RenderedMaterial.SetTexture(Albedo, 0);
	auto* Normal = GetTextureCache()->LoadTexture("Resources/brickwork-normal.jpg");
	TestMesh->RenderedMaterial.SetTexture(Normal, 1);
	
	MeshRenderingComponent* OtherMesh = Test->CreateEntity("Other test entity")->AddComponent<MeshRenderingComponent>();
	OtherMesh->GetOwner()->GetPhysicsProperties().Mass = 1000.;
	OtherMesh->GetOwner()->GetTransform()->Scale(0.1f);
	OtherMesh->RenderedMesh = Test->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
	OtherMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
	OtherMesh->GetTransform()->SetPosition(float3(1., 2., 3.));

	MeshRenderingComponent* OtherMesh2 = Test->CreateEntity("Other test entity 2")->AddComponent<MeshRenderingComponent>();
	OtherMesh2->GetOwner()->GetTransform()->Scale(0.1f);
	OtherMesh2->GetOwner()->GetPhysicsProperties().Mass = 1000.;
	OtherMesh2->RenderedMesh = Test->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
	OtherMesh2->RenderedMaterial.InitializeMaterial("MainShader.gfx");
	OtherMesh2->GetTransform()->SetPosition(float3(1., 2., -3.));

	MeshRenderingComponent* OtherMesh3 = Test->CreateEntity("Other test entity 3")->AddComponent<MeshRenderingComponent>();
	OtherMesh3->GetOwner()->GetPhysicsProperties().Mass = 1000.;
	OtherMesh3->GetOwner()->GetTransform()->Scale(2871000000.f);
	OtherMesh3->RenderedMesh = Test->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
	OtherMesh3->RenderedMaterial.InitializeMaterial("MainShader.gfx");
	OtherMesh3->GetTransform()->SetPosition(float3(1.f, 2.f, -3.f));

	Entity* MainCamera = Test->CreateEntity("TestCamera");
	MainCamera->GetPhysicsProperties().bCanTick = false;
	MainCamera->GetTransform()->SetPosition(float3(0.f, 0.f, -2.f));
	auto* SceneCamera = MainCamera->AddComponent<CameraComponent>();
	SceneCamera->ZNear = 1E-2f;
	SceneCamera->ZFar = 1E+17f;
	MainCamera->AddComponent<CameraMovementComponent>()->Speed = 1.f;

	auto* Skybox = GetTextureCache()->LoadCubemap("Testskybox", "Resources/nx.png", "Resources/ny.png", "Resources/nz.png", "Resources/px.png", "Resources/py.png", "Resources/pz.png");
	Test->SetSkybox(Skybox);
	std::cout << "Application Initialized" << std::endl;
}

void InitApplication()
{
	GetSceneManager()->CreateNewScene("Space Scene");
	Scene* SpaceLevel = GetSceneManager()->CurrentScene;
	{
		Entity* Sun = SpaceLevel->CreateEntity("Sun");
		Sun->GetPhysicsProperties().Mass = 1.989e30f;
		auto* SunMesh = Sun->AddComponent<MeshRenderingComponent>();
		SunMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
		SunMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		Sun->GetTransform()->Scale(1391016.0f);
	}
	Entity* FollowedPlanet = nullptr;
	{
		Entity* Mercury = SpaceLevel->CreateEntity("Mercury");
		Mercury->GetPhysicsProperties().Mass = 3.285e23f;
		Mercury->GetPhysicsProperties().Velocity = float3((GRAV_CONST * 1.989e30f) / 57910000.f, 0.f, 0.f);
		Mercury->GetTransform()->SetPosition(float3(0.f, 0.f, -57910000.f));
		auto* MercuryMesh = Mercury->AddComponent<MeshRenderingComponent>();
		MercuryMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
		MercuryMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		Mercury->GetTransform()->Scale(4879.f);
		FollowedPlanet = Mercury;
	}
	Entity* CamEntity = SpaceLevel->CreateEntity("Camera");
	CamEntity->GetTransform()->SetPosition(float3(0.f, 0.f, -2.f));
	CamEntity->GetPhysicsProperties().bCanTick = false;
	auto* Cam = CamEntity->AddComponent<CameraComponent>();
	Cam->ZNear = 1e-2f;
	Cam->ZFar = 1e+18f;
	CamEntity->AddComponent<CameraMovementComponent>()->Speed = 1e7f;
	CamEntity->AddComponent<CameraFollowPlanet>()->ParentPlanet = FollowedPlanet;
	{

	}
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



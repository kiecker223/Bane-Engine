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
		double3 PlanetPos = ParentPlanet->GetTransform()->GetPosition();
		if (length(PlanetPos - GetTransform()->GetPosition()) > 2e4)
		{
			GetTransform()->SetPosition(PlanetPos + double3(0., 0., -3000.0));
		}
	}

};

class PrintVelocityFromPhysicsComponent : public Component
{
	IMPLEMENT_COMPONENT(PrintVelocityFromPhysicsComponent)
public:

	void Tick(float Dt) override 
	{
		UNUSED(Dt);
		auto Vel = GetOwner()->GetPhysicsProperties().Velocity;
		std::cout << Vel.x << " : " << Vel.y << " : " << Vel.z << std::endl;
	}
};

void InitApplication2()
{
	abort();
	/*
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
	*/
}

void InitApplication()
{
	GetSceneManager()->CreateNewScene("Space Scene");
	Scene* SpaceLevel = GetSceneManager()->CurrentScene;

	{
		Entity* Sun = SpaceLevel->CreateEntity("Sun");
		Sun->GetPhysicsProperties().Mass = 1.989e30;
		auto* SunMesh = Sun->AddComponent<MeshRenderingComponent>();
		SunMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
		SunMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		Sun->GetTransform()->Scale(1391016000.0);
	}
	Entity* FollowedPlanet = nullptr;
	{
		double DFS = 57910000000.;
		Entity* Mercury = SpaceLevel->CreateEntity("Mercury");
		Mercury->GetPhysicsProperties().Mass = 3.285e23;
		Mercury->GetPhysicsProperties().Velocity = double3((M_GRAV_CONST * 1.989e30) / DFS, 0., 0.);
		Mercury->GetPhysicsProperties().bCanTick = false;
		Mercury->GetTransform()->SetPosition(double3(0., 0., -DFS));
		auto* MercuryMesh = Mercury->AddComponent<MeshRenderingComponent>();
		MercuryMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
		MercuryMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		Mercury->GetTransform()->Scale(4879000.0);
	}
	{
		double DFS = 108200000.;
		Entity* Venus = SpaceLevel->CreateEntity("Venus");
		Venus->GetPhysicsProperties().Mass = 4.867e24f;
		Venus->GetPhysicsProperties().Velocity = double3(0., 0.0, 0.0);
		Venus->GetPhysicsProperties().bCanTick = false;
		Venus->GetTransform()->SetPosition(double3(0.f, 0.f, -DFS));
		auto* VenusMesh = Venus->AddComponent<MeshRenderingComponent>();
		VenusMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
		VenusMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		Venus->GetTransform()->Scale(12104000.);
	}
	{
		Entity* Earth = SpaceLevel->CreateEntity("Earth");
		Earth->GetPhysicsProperties().Mass = 5.97219e24;
		Earth->GetPhysicsProperties().Velocity = double3(30000.0, 0., 0.);
		//Earth->GetPhysicsProperties().bCanTick = false;
		auto Test = Earth->GetPhysicsProperties().Velocity;
		UNUSED(Test);
		Earth->GetTransform()->SetPosition(double3(0., 0., -M_AU(1.)));
		auto* EarthMesh = Earth->AddComponent<MeshRenderingComponent>();
		EarthMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
		EarthMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		EarthMesh->RenderedMaterial.SetTexture("Resources/EarthNoClouds.jpg", 0);
		Earth->GetTransform()->Scale(12742000.0);
		Earth->AddComponent<PrintVelocityFromPhysicsComponent>();

		Entity* Luna = SpaceLevel->CreateEntity("Luna");
		Luna->GetPhysicsProperties().Mass = 7.34767309e22;
		Luna->GetPhysicsProperties().Velocity = double3((M_GRAV_CONST * 1.989e30 / M_AU(1.) + 384400000.), 0., 0.);
		Luna->GetPhysicsProperties().bCanTick = false;
		Luna->GetTransform()->SetPosition(double3(0., 0., M_AU(1.) + 384400000.));
		auto* LunaMesh = Luna->AddComponent<MeshRenderingComponent>();
		LunaMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Meshes/sphere.FBX");
		LunaMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		Luna->GetTransform()->Scale(3474000.0);
		FollowedPlanet = Earth;
	}
	Entity* CamEntity = SpaceLevel->CreateEntity("Camera");
	CamEntity->GetTransform()->SetPosition(double3(0., 0., -M_AU(1.)));
	CamEntity->GetPhysicsProperties().bCanTick = false;
	auto* Cam = CamEntity->AddComponent<CameraComponent>();
	Cam->ZNear = 1e-2f;
	Cam->ZFar = 1e+21f;
	CamEntity->AddComponent<CameraMovementComponent>()->Speed = 1e7;
	//CamEntity->AddComponent<CameraFollowPlanet>()->ParentPlanet = FollowedPlanet;
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



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

	CameraFollowPlanet() { }

	Entity* ParentPlanet;
	TComponentHandle<CameraComponent> CamComp;

	void Start()
	{
		CamComp = GetOwner()->GetComponent<CameraComponent>();
	}

	void Tick(float Dt) override
	{
		UNUSED(Dt);
		double3 PlanetPos = ParentPlanet->GetTransform()->GetPosition();
		GetTransform()->SetPosition(PlanetPos + double3(-30000000., 0., -70000000.0));
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

class RotateEarthComponent : public Component
{
	IMPLEMENT_COMPONENT(RotateEarthComponent)
public:

	void Tick(float Dt) override
	{
		GetTransform()->Rotate(float3(0.f, 1.f * Dt, 0.f));
	}
};



void InitApplication()
{
	GetSceneManager()->CreateNewScene("Space Scene");
	Scene* SpaceLevel = GetSceneManager()->CurrentScene;
	{
		Mesh* AllocMesh = SpaceLevel->GetMeshCache().AllocateMesh();
		AllocMesh->GenerateUVSphere(32);
		SpaceLevel->GetMeshCache().SaveMesh(AllocMesh, "Sphere");
	}
	{
		Entity* Sun = SpaceLevel->CreateEntity("Sun");
		Sun->GetPhysicsProperties().Mass = 1.989e30;
		auto SunMesh = Sun->AddComponent<MeshRenderingComponent>();
		SunMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Meshes/Sphere.FBX");
		SunMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		SunMesh->RenderedMaterial.SetTexture("Resources/8k_sun.jpg", 0);
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
		auto MercuryMesh = Mercury->AddComponent<MeshRenderingComponent>();
		MercuryMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		MercuryMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MercuryMesh->RenderedMaterial.SetTexture("Resources/2k_mercury.jpg", 0);
		Mercury->GetTransform()->Scale(4879000.0);
	}
	{
		double DFS = 108200000000.;
		Entity* Venus = SpaceLevel->CreateEntity("Venus");
		Venus->GetPhysicsProperties().Mass = 4.867e24f;
		Venus->GetPhysicsProperties().Velocity = double3(0., 0.0, 0.0);
		Venus->GetPhysicsProperties().bCanTick = false;
		Venus->GetTransform()->SetPosition(double3(0.f, 0.f, -DFS));
		auto VenusMesh = Venus->AddComponent<MeshRenderingComponent>();
		VenusMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		VenusMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		VenusMesh->RenderedMaterial.SetTexture("Resources/8k_venus_atmosphere.jpg", 0);
		Venus->GetTransform()->Scale(12104000.);
	}
	{
		Entity* Earth = SpaceLevel->CreateEntity("Earth");
		Earth->GetPhysicsProperties().Mass = 5.97219e24;
		Earth->GetPhysicsProperties().Velocity = double3(30000.0 * (1. / 60.), 0., 0.);
		Earth->GetPhysicsProperties().bCanTick = false;
		Earth->GetTransform()->SetPosition(double3(0., 0., -M_AU(1.)));
		auto EarthMesh = Earth->AddComponent<MeshRenderingComponent>();
		EarthMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		EarthMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		EarthMesh->RenderedMaterial.SetTexture("Resources/EarthNoClouds.jpg", 0);
		Earth->GetTransform()->Scale(12742000.0);

		Entity* Luna = SpaceLevel->CreateEntity("Luna");
		Luna->GetPhysicsProperties().Mass = 7.34767309e22;
		Luna->GetPhysicsProperties().Velocity = double3((M_GRAV_CONST * 1.989e30 / M_AU(1.) + 384400000.), 0., 0.);
		//Luna->GetPhysicsProperties().bCanTick = false;
		Luna->GetTransform()->SetPosition(double3(0., 0., M_AU(1.) + 384400000.));
		auto LunaMesh = Luna->AddComponent<MeshRenderingComponent>();
		LunaMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		LunaMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		Luna->GetTransform()->Scale(3474000.0);
		FollowedPlanet = Earth;
	}

	Entity* CamEntity = SpaceLevel->CreateEntity("Camera");
	CamEntity->GetTransform()->SetPosition(double3(0., 0., -M_AU(1.0)));
	CamEntity->GetPhysicsProperties().bCanTick = false;
	CamEntity->GetTransform()->SetRotation(float3(0.f, 0.f, 0.f));
	auto Cam = CamEntity->AddComponent<CameraComponent>();
	Cam->ZNear = 1e-2f;
	Cam->ZFar = 1e+21f;
	CamEntity->AddComponent<CameraMovementComponent>()->Speed = 1e7;
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



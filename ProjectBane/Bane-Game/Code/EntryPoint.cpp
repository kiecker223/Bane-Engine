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


double KeplerStart(double Eccentricity, double M)
{
	double t34 = Eccentricity * Eccentricity;
	double t35 = t34 * Eccentricity;
	double t33 = cos(M);
	return M + (-1. / 2.*t35 + Eccentricity + (t34 + 3. / 3.*t33*t35)*t33)*sin(M);
}

double Eps3(double Eccentricity, double M, double StartingValue)
{
	double t1 = cos(StartingValue);
	double t2 = -1. + Eccentricity * t1;
	double t3 = sin(StartingValue);
	double t4 = Eccentricity * t3;
	double t5 = -StartingValue + t4 + M;
	double t6 = t5 / (0.5*t5*t4 / t2 + t2);
	return t5 / ((0.5*t3 - 1. / 6.*t1*t6)*Eccentricity*t6 + t2);
}

double SolvePassageOfPericenter()
{
	return 0.0;
}

double SolveEccentricAnomaly(double Time, double Period, double Eccentricity, double TimePeriod)
{
	double Tolerance;
	if (Eccentricity < 0.8)
	{
		Tolerance = 1e-14;
	}
	else
	{
		Tolerance = 1e-13;
	}
	double n = 2 * _M_PI_ / Period;
	double MeanAnomaly = n * (Time - TimePeriod);
	double MNorm = fmod(MeanAnomaly, 2. * _M_PI_);
	double E0 = KeplerStart(Eccentricity, MNorm);
	double dE = Tolerance + 1;
	double E = 0.;
	uint32 Iter = 0;
	while (dE > Tolerance)
	{
		E = E0 - Eps3(Eccentricity, MNorm, E0);
		dE = abs(E - E0);
		E0 = E;
		Iter++;
		if (Iter == 100) break;
	}
	return E;
}

typedef struct PLANET_START_INFO {
	double3 Position;
	double3 Velocity;
} PLANET_START_INFO;

typedef struct PLANET_ORBIT_INFO {
	double Periapsis;
	double Apoapsis;
	double OribitalPeriod;
	double Eccentricity;
	double LongitudeOfAscendingNode;
	double ArgumentOfPerigee;
	double Inclination;
} PLANET_ORBIT_INFO;

PLANET_START_INFO PlacePlanet(const PLANET_ORBIT_INFO& OrbitInfo, double Time)
{
	PLANET_START_INFO Result;
	double SemimajorAxis = (OrbitInfo.Periapsis + OrbitInfo.Apoapsis) / 2.;
	double SemiminorAxis = SemimajorAxis * sqrt(1 - (OrbitInfo.Eccentricity * OrbitInfo.Eccentricity));
	double EccentricAnomaly = SolveEccentricAnomaly(Time, OrbitInfo.OribitalPeriod, OrbitInfo.Eccentricity, 0);
	Result.Position.x = SemimajorAxis * (cos(EccentricAnomaly) - OrbitInfo.Eccentricity);
	Result.Position.z = SemiminorAxis * sin(EccentricAnomaly);
	return Result;
}


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
		SunMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		SunMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		SunMesh->RenderedMaterial.SetTexture("Resources/8k_sun.jpg", 0);
		Sun->GetTransform()->Scale(1391016000.0);
		//FollowedPlanet = Sun;
	}
	Entity* FollowedPlanet = nullptr;
	
	const double TimeArg = 1000.;
	{
		PLANET_START_INFO SInfo = PlacePlanet({ 460008700000.0, 69817332000.0, 88., 0.2056, 48.3396, 7.005 }, TimeArg);
		Entity* Mercury = SpaceLevel->CreateEntity("Mercury");
		Mercury->GetPhysicsProperties().Mass = 3.285e23;
		//Mercury->GetPhysicsProperties().Velocity = double3((M_GRAV_CONST * 1.989e30) / DFS, 0., 0.);
		Mercury->GetPhysicsProperties().bCanTick = false;
		Mercury->GetTransform()->SetPosition(SInfo.Position);
		auto MercuryMesh = Mercury->AddComponent<MeshRenderingComponent>();
		MercuryMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		MercuryMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MercuryMesh->RenderedMaterial.SetTexture("Resources/2k_mercury.jpg", 0);
		Mercury->GetTransform()->Scale(4879000.0);
	}
	{
		PLANET_START_INFO SInfo = PlacePlanet({ 107475372000.0, 108939198000.0, 224.69, 0.006, 76.6726, 3.3977 }, TimeArg);
		Entity* Venus = SpaceLevel->CreateEntity("Venus");
		Venus->GetPhysicsProperties().Mass = 4.867e24f;
		Venus->GetPhysicsProperties().Velocity = double3(0., 0.0, 0.0);
		Venus->GetPhysicsProperties().bCanTick = false;
		Venus->GetTransform()->SetPosition(SInfo.Position);
		std::cout << SInfo.Position.x << " : " << SInfo.Position.y << " : " << SInfo.Position.z << std::endl;
		std::cout << length(SInfo.Position) << std::endl;
		auto VenusMesh = Venus->AddComponent<MeshRenderingComponent>();
		VenusMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		VenusMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		VenusMesh->RenderedMaterial.SetTexture("Resources/8k_venus_atmosphere.jpg", 0);
		Venus->GetTransform()->Scale(12104000.);
		//FollowedPlanet = Venus;
	}
	{
		Entity* Earth = SpaceLevel->CreateEntity("Earth");
		PLANET_START_INFO SInfo = PlacePlanet({ 147094882000.0, 152100915000.0, 365.25, 0.016, 0, 0 }, TimeArg);
		Earth->GetPhysicsProperties().Mass = 5.97219e24;
		Earth->GetPhysicsProperties().Velocity = double3(30000.0 * (1. / 60.), 0., 0.);
		Earth->GetPhysicsProperties().AngularVelocity = double3(0., 1., 0.) * (7.2921159e-5 * (1. / 60.));
		Earth->GetTransform()->SetPosition(SInfo.Position);
		std::cout << "Earth Position : " << SInfo.Position.x << " : " << SInfo.Position.y << " : " << SInfo.Position.z << std::endl;
		std::cout << "Earth Distance : " << length(SInfo.Position) << std::endl;
		auto EarthMesh = Earth->AddComponent<MeshRenderingComponent>();
		EarthMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		EarthMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		EarthMesh->RenderedMaterial.SetTexture("Resources/EarthNoClouds.jpg", 0);
		Earth->GetTransform()->Scale(12742000.0);

		Entity* Luna = SpaceLevel->CreateEntity("Luna");
		Luna->GetPhysicsProperties().Mass = 7.34767309e22;
		Luna->GetPhysicsProperties().Velocity = double3((M_GRAV_CONST * 1.989e30 / M_AU(1.) + 384400000.), 0., 0.);
		Luna->GetTransform()->SetPosition(double3(0., 0., M_AU(1.) + 384400000.));
		auto LunaMesh = Luna->AddComponent<MeshRenderingComponent>();
		LunaMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		LunaMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		Luna->GetTransform()->Scale(3474000.0);
		FollowedPlanet = Earth;
	}

	Entity* CamEntity = SpaceLevel->CreateEntity("Camera");
	CamEntity->GetTransform()->SetPosition(FollowedPlanet->GetTransform()->GetPosition());
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



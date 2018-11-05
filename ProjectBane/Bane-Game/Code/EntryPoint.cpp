#include "BaneObject/Scene/SceneManager.h"
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"
#include <iostream>
#include "Core/Containers/HeapQueue.h"
#include "Graphics/IO/TextureCache.h"
#include "BaneObject/CoreComponents/CameraComponent.h"
#include "SwapParentPlanet.h"
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

/*
	N = longitude of the ascending node
	i = inclination to the ecliptic (plane of the Earth's orbit)
	w = argument of perihelion
	a = semi-major axis, or mean distance from Sun
	e = eccentricity (0=circle, 0-1=ellipse, 1=parabola)
	M = mean anomaly (0 at perihelion; increases uniformly with time)
*/

typedef struct PLANET_ORBIT_INFO {
	std::string PlanetName;
	double LongitudeOfAscendingNode;
	double InclinationOfEcliptic;
	double ArgumentOfPerihelion;
	double SemimajorAxis;
	double Eccentricity;
	double MeanAnomaly;
} PLANET_ORBIT_INFO;

double3 SunPosition = double3(M_NEGATIVE_INFINITY, M_NEGATIVE_INFINITY, M_NEGATIVE_INFINITY);
const double SunMass = 1.989e30;

double3 EarthVelocity;
const double EarthMass = 5.97219e24;

/*
	N = longitude of the ascending node
	i = inclination to the ecliptic (plane of the Earth's orbit)
	w = argument of perihelion
	a = semi-major axis, or mean distance from Sun
	e = eccentricity (0=circle, 0-1=ellipse, 1=parabola)
	M = mean anomaly (0 at perihelion; increases uniformly with time)

	w1 = N + w   = longitude of perihelion
	L  = M + w1  = mean longitude
	q  = a*(1-e) = perihelion distance
	Q  = a*(1+e) = aphelion distance
	P  = a ^ 1.5 = orbital period (years if a is in AU, astronomical units)
	T  = Epoch_of_M - (M(deg)/360_deg) / P  = time of perihelion
	v  = true anomaly (angle between position and perihelion)
	E  = eccentric anomaly
*/



PLANET_START_INFO PlacePlanet(PLANET_ORBIT_INFO OrbitInfo, double Time)
{
	PLANET_START_INFO Result;
	UNUSED(Time);
	Result.Position = double3(0.0, 0.0, 0.0);

	
	OrbitInfo.InclinationOfEcliptic = radiansD(OrbitInfo.InclinationOfEcliptic);
	if (OrbitInfo.PlanetName != "Earth")
	{
		double EccentricAnomaly = OrbitInfo.MeanAnomaly + (OrbitInfo.Eccentricity * (180.0 / _M_PI_)) * sin(OrbitInfo.MeanAnomaly) * (1.0 + OrbitInfo.Eccentricity * cos(OrbitInfo.MeanAnomaly));
		double E0 = EccentricAnomaly;
		double E1 = 0.;
		if (OrbitInfo.Eccentricity > 0.06)
		{
			for (uint32 i = 0; i < 100; i++)
			{
				E1 = E0 - (E0 - (OrbitInfo.Eccentricity*(180./_M_PI_)) * sin(E0) - OrbitInfo.MeanAnomaly) / (1 - OrbitInfo.Eccentricity * cos(E0));
				if (abs(E1 - E0) <= 1E-4)
				{
					break;
				}
			}
			EccentricAnomaly = E0;
		}
		double xv = OrbitInfo.SemimajorAxis * cos(EccentricAnomaly) - OrbitInfo.Eccentricity;
		double yv = OrbitInfo.SemimajorAxis * sqrt(1.0 - OrbitInfo.Eccentricity * OrbitInfo.Eccentricity) * sin(EccentricAnomaly);
		double TrueAnomaly = atan2(yv, xv);
		double Distance = sqrt(xv * xv + yv * yv);
		double SinVW = sin(TrueAnomaly + OrbitInfo.ArgumentOfPerihelion);
		double CosVW = cos(TrueAnomaly + OrbitInfo.ArgumentOfPerihelion);
		double CosI = cos(OrbitInfo.LongitudeOfAscendingNode);
		double SinI = sin(OrbitInfo.LongitudeOfAscendingNode);
		double CosInclination = cos(OrbitInfo.InclinationOfEcliptic);
		Result.Position = double3(
			Distance * (CosI * CosVW - SinI * SinVW * CosInclination),
			Distance * (SinVW * sin(OrbitInfo.InclinationOfEcliptic)),
			Distance * (SinI * CosVW + CosI * SinVW * CosInclination)
		);
	}
	if (OrbitInfo.PlanetName == "Sun")
	{
		SunPosition = Result.Position;
		Result.Position = double3(0.0, 0.0, 0.0);
		return Result;
	}
	bool bIsEarth = OrbitInfo.PlanetName == "Earth";
	bool bIsMoon = OrbitInfo.PlanetName == "Moon";
	if (bIsEarth || bIsMoon)
	{
		Result.Position -= SunPosition;
	}
	
	if (OrbitInfo.PlanetName != "Sun")
	{
		BANE_CHECK(!isNan(Result.Position));
		if (!bIsMoon)
		{
			double3 RightVector = normalized(cross(normalized(Result.Position), double3(0., 1., 0.)));
			double TwoOverDistance = 2.0 / length(Result.Position);
			double OneOverA = 1.0 / OrbitInfo.SemimajorAxis;
			double Power = sqrt((1.3275978e+20) * (TwoOverDistance) - (OneOverA));
			Result.Velocity = RightVector * Power;
		}
		else
		{
			double3 RightVector = normalized(cross(normalized(Result.Position - SunPosition), double3(0.0, 1.0, 0.0)));
			double TwoOverDistance = 2.0 / length(Result.Position - SunPosition);
			double OneOverA = 1.0 / OrbitInfo.SemimajorAxis;
			double Power = sqrt((M_GRAV_CONST * EarthMass) * (TwoOverDistance)-(OneOverA));
			Result.Velocity = (RightVector * Power) + EarthVelocity;
		}
		if (bIsEarth)
		{
			EarthVelocity = Result.Velocity;
		}

		std::cout << "Planet: " << OrbitInfo.PlanetName << " velocity: " << Result.Velocity.x << " " << Result.Velocity.y << " " << Result.Velocity.z << std::endl;
		std::cout << "Velocity power: " << length(Result.Velocity) << std::endl;
	}
	return Result;
}


std::string SomeRandoNames[] =
{
	std::string("asdfl;"),
	std::string("asdfas"),
	std::string("asdfae"),
	std::string("jkghjk"),
	std::string("7rtyue"),
	std::string("yurtyj"),
	std::string("jrtyjf"),
	std::string("tyjdfg"),
	std::string("dfghnt"),
	std::string("ner5ye"),
	std::string("nerty7"),
	std::string("the4r5"),
	std::string("45yrth"),
	std::string("asdfl;"),
	std::string("asdfas"),
	std::string("asdfae"),
	std::string("jkghjk"),
	std::string("7rtyue"),
	std::string("yurtyj"),
	std::string("jrtyjf"),
	std::string("tyjdfg"),
	std::string("dfghnt"),
	std::string("ner5ye"),
	std::string("nerty7"),
	std::string("the4r5"),
	std::string("45yrth"),
	std::string("asdfl;"),
	std::string("asdfas"),
	std::string("asdfae"),
	std::string("jkghjk"),
	std::string("7rtyue"),
	std::string("yurtyj"),
	std::string("jrtyjf"),
	std::string("tyjdfg"),
	std::string("dfghnt"),
	std::string("ner5ye"),
	std::string("nerty7"),
	std::string("the4r5"),
	std::string("45yrth"),
	std::string("asdfl;"),
	std::string("asdfas"),
	std::string("asdfae"),
	std::string("jkghjk"),
	std::string("7rtyue"),
	std::string("yurtyj"),
	std::string("jrtyjf"),
	std::string("tyjdfg"),
	std::string("dfghnt"),
	std::string("ner5ye"),
	std::string("nerty7"),
	std::string("the4r5"),
	std::string("45yrth")
};

struct BigBoiStruct
{
	uint32 Elements[100];
	//std::string OtherShit;

	static BigBoiStruct Current()
	{
		static BigBoiStruct Result;
		Result.Elements[Result.Elements[0] % 100]++;
		Result.Elements[0]++;
		//Result.OtherShit = SomeRandoNames[Result.Elements[0]];
		return Result;
	}
};

void InitApplication2()
{
	UNUSED(1);
	UNUSED(2);
	TArray<BigBoiStruct> OtherValues;
	UNUSED(3);
	{
		TArray<BigBoiStruct> Values;
		for (uint32 i = 0; i < 40; i++)
		{
			Values.Add(BigBoiStruct::Current());
		}
		OtherValues = Values;
	}
	OtherValues.ClearMemory();
	{
		TArray<BigBoiStruct> Values;
		Values += BigBoiStruct::Current();
		Values += BigBoiStruct::Current();
		Values += BigBoiStruct::Current();
		OtherValues = Values;
	}
	OtherValues.ClearMemory();
	UNUSED(1);
	UNUSED(2);
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
	std::cout << "Astronomical unit check: " << M_AU(1.0) << std::endl;
	const double TimeArg = 9000.;
	{
		PLANET_START_INFO SInfo = PlacePlanet(
			{
				"Sun",
				0.0, 
				0.0, 
				282.9404 + 4.70935E-5 * TimeArg,
				M_AU(1.0), 
				0.016709 - 1.151E-9*TimeArg,
				356.0470 + 0.9856002585 * TimeArg
			},
			TimeArg);
		Entity* Sun = SpaceLevel->CreateEntity("Sun");
		std::cout << "Sun position: " << SInfo.Position.x << " " << SInfo.Position.y << " " << SInfo.Position.z << std::endl;
		Sun->GetTransform()->SetPosition(SInfo.Position);
		Sun->GetPhysicsProperties().Mass = 1.989e30;
		auto SunMesh = Sun->AddComponent<MeshRenderingComponent>();
		SunMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		SunMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		SunMesh->RenderedMaterial.SetTexture("Resources/8k_sun.jpg", 0);
		Sun->GetTransform()->Scale(1391016000.0);
		//FollowedPlanet = Sun;
	}
	Entity* FollowedPlanet = nullptr;
	{
		PLANET_START_INFO SInfo = PlacePlanet(
			{
				"Mercury",
				48.3313 + 3.24587E-5 * TimeArg,
				7.0047 + 5.00E-8 * TimeArg,
				29.1241 + 1.01444E-5 * TimeArg, 
				M_AU(0.397098),
				0.205635 + 5.59E-10 * TimeArg,
				168.6562 + 4.0923344368 * TimeArg
			},
			TimeArg
		);
		Entity* Mercury = SpaceLevel->CreateEntity("Mercury");
		std::cout << "Mercury Distance: " << length(SInfo.Position) << std::endl;
		Mercury->GetPhysicsProperties().Mass = 3.285e23;
		Mercury->GetPhysicsProperties().Velocity = SInfo.Velocity * (1.0 / 60.);
		//Mercury->GetPhysicsProperties().bCanTick = false;
		Mercury->GetTransform()->SetPosition(SInfo.Position);
		auto MercuryMesh = Mercury->AddComponent<MeshRenderingComponent>();
		MercuryMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		MercuryMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MercuryMesh->RenderedMaterial.SetTexture("Resources/2k_mercury.jpg", 0);
		Mercury->GetTransform()->Scale(4879000.0);
	}
	{
		PLANET_START_INFO SInfo = PlacePlanet(
			{ 
				"Venus", 
				76.6799 + 2.46590E-5 * TimeArg, 
				3.3946 + 2.75E-8 * TimeArg, 
				54.8910 + 1.38374E-5 * TimeArg,
				M_AU(0.723330),	
				0.006773 - 1.302E-9 * TimeArg, 
				48.0052 + 1.6021302244 * TimeArg 
			}, 
			TimeArg
		);
		std::cout << "Venus distance: " << length(SInfo.Position) << std::endl;
		Entity* Venus = SpaceLevel->CreateEntity("Venus");
		Venus->GetPhysicsProperties().Mass = 4.867e24f;
		Venus->GetPhysicsProperties().Velocity = SInfo.Velocity * (1.0 / 60.0);
		Venus->GetTransform()->SetPosition(SInfo.Position);
		auto VenusMesh = Venus->AddComponent<MeshRenderingComponent>();
		VenusMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		VenusMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		VenusMesh->RenderedMaterial.SetTexture("Resources/8k_venus_atmosphere.jpg", 0);
		Venus->GetTransform()->Scale(12104000.);
		//FollowedPlanet = Venus;
	}
	{
		Entity* Earth = SpaceLevel->CreateEntity("Earth");
		PLANET_START_INFO SInfo = PlacePlanet({ "Earth", 0., 0., 0., M_AU(1.0), 0., 0. }, TimeArg);
		std::cout << "Earth Distance: " << length(SInfo.Position) << std::endl;
		Earth->GetPhysicsProperties().Mass = 5.97219e24;
		Earth->GetPhysicsProperties().Velocity = SInfo.Velocity * (1.0 / 60.0);
		Earth->GetPhysicsProperties().AngularVelocity = double3(0., 1., 0.) * (7.2921159e-5 * (1. / 60.));
		Earth->GetTransform()->SetPosition(SInfo.Position);
		auto EarthMesh = Earth->AddComponent<MeshRenderingComponent>();
		EarthMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		EarthMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		EarthMesh->RenderedMaterial.SetTexture("Resources/EarthNoClouds.jpg", 0);
		Earth->GetTransform()->Scale(12742000.0);
		FollowedPlanet = Earth;
	}
	{
		PLANET_START_INFO SInfo = PlacePlanet(
			{
				"Moon",
				125.1228 - 0.0529538083 * TimeArg,
				5.1454,
				318.0634 + 0.1643573223 * TimeArg,
				60.266 * (12742000.0 / 2.),
				0.054900,
				1163654 + 13.069929509 * TimeArg
			},
			TimeArg
		);
		Entity* Luna = SpaceLevel->CreateEntity("Luna");
		Luna->GetPhysicsProperties().Mass = 7.34767309e22;
		Luna->GetPhysicsProperties().Velocity = SInfo.Velocity * (1.0 / 60.0);
		Luna->GetTransform()->SetPosition(SInfo.Position);
		auto LunaMesh = Luna->AddComponent<MeshRenderingComponent>();
		LunaMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		LunaMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		LunaMesh->RenderedMaterial.SetTexture("Resources/8k_moon.jpg", 0);
		Luna->GetTransform()->Scale(3474000.0);
	}

	Entity* CamEntity = SpaceLevel->CreateEntity("Camera");
	CamEntity->GetTransform()->SetPosition(FollowedPlanet->GetTransform()->GetPosition());
	CamEntity->GetPhysicsProperties().bCanTick = false;
	CamEntity->GetTransform()->SetRotation(float3(0.f, 0.f, 0.f));
	auto Cam = CamEntity->AddComponent<CameraComponent>();
	Cam->ZNear = 1e-2f;
	Cam->ZFar = 1e+21f; 
	CamEntity->AddComponent<CameraMovementComponent>()->Speed = 3e7;
	CamEntity->AddComponent<SwapParentPlanet>();
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



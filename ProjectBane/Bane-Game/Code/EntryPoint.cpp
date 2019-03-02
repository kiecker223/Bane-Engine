
#include "../Bane-Executable/Executable/Application.h"
#include "BaneObject/Scene/SceneManager.h"
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"
#include <iostream>
#include "Core/Containers/HeapQueue.h"
#include "Graphics/IO/TextureCache.h"
#include "BaneObject/CoreComponents/SphereCollisionComponent.h"
#include "BaneObject/CoreComponents/CameraComponent.h"
#include "SwapParentPlanet.h"
#include "UI/Button.h"
#include "CameraMovementComponent.h"
#include "BaneObject/CoreComponents/CylinderCollisionComponent.h"
#include "RaycastTestComponent.h"
#include "Graphics/IO/ShaderCache.h"



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

	void Tick(double Dt) override
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

	void Tick(double Dt) override 
	{
		UNUSED(Dt);
	}
};

class RotateEarthComponent : public Component
{
	IMPLEMENT_COMPONENT(RotateEarthComponent)
public:

	void Tick(double Dt) override
	{
		GetTransform()->Rotate(double3(0., 1. * Dt, 0.));
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

class TestUIComponent : public RenderComponent
{
	IMPLEMENT_COMPONENT(TestUIComponent)

public:

	TestUIComponent() { }

	void Start()
	{
		Context.Initialize();
		Button.Initialize();
		Button.Text = "H";
	}

	void GraphicsUpdate(RenderLoop& RL)
	{
		RL.PostRenderCallback.Push([this](IGraphicsDevice* pDevice, IGraphicsCommandContext* CommandContext)
		{
			Context.BeginFrame(CommandContext, nullptr);
			Button.Render(Context);
		});
	}

	UIButton Button;
	UIContext Context;
};

class TestDummyComponent : public Component
{
	IMPLEMENT_COMPONENT(TestDummyComponent)
public:

	void Tick(double DT)
	{
	}
};

class TestRenderingMeshComponent : public Component
{
	IMPLEMENT_COMPONENT(TestRenderingMeshComponent)

public:

	double TimeLeft = 5.0;
	uint32 NumMeshes;
	Mesh* pMesh;

	void Start()
	{
		pMesh = GetScene()->GetMeshCache().LoadMesh("Sphere");
	}

	void Tick(double Dt)
	{
		if (NumMeshes > 120) return;
		TimeLeft -= Dt;
		if (TimeLeft <= 0)
		{
			NumMeshes++;
			Entity* NewEntity = GetScene()->CreateEntity(std::string("Entity Num: ") + std::to_string(NumMeshes));
			auto Mrc = NewEntity->AddComponent<MeshRenderingComponent>();
			Mrc->RenderedMesh = pMesh;
			Mrc->RenderedMaterial.InitializeMaterial("MainShader.gfx");
			Mrc->RenderedMaterial.SetDiffuseTexture("DefaultBlue");
			auto Collision = NewEntity->AddComponent<SphereCollisionComponent>();
			Collision->SetMass(100000000);
			Collision->SetRadius(0.03);
			double DNumMeshes = static_cast<double>(NumMeshes);
			NewEntity->GetTransform()->SetPosition(double3(DNumMeshes + 1.1, 40.0 - DNumMeshes, (40.0 - DNumMeshes) / 30.0));
			TimeLeft = 0.001;
		}
	}

};


void RenderJob(IGraphicsCommandBuffer* CmdBuff)
{

}

void JobSystemTestForGame()
{
	Window* pWindow = GetApplicationInstance()->GetWindow();
	ApiRuntime* Runtime = GetApiRuntime();
	IGraphicsDevice* GraphicsDevice = Runtime->GetGraphicsDevice();

	auto* BackPass = GraphicsDevice->GetBackBufferTargetPass();

	ITexture2D* pTex = GraphicsDevice->CreateTexture2D(1920, 1080, FORMAT_R8G8B8A8_UNORM, CreateDefaultSamplerDesc(), TEXTURE_USAGE_RENDER_TARGET | TEXTURE_USAGE_SHADER_RESOURCE, nullptr);
	IRenderTargetView* pView = GraphicsDevice->CreateRenderTargetView(pTex);
	ITexture2D* pDepthTex = GraphicsDevice->CreateTexture2D(1920, 1080, FORMAT_D24_UNORM_S8_UINT, CreateDefaultSamplerDesc(), TEXTURE_USAGE_DEPTH_STENCIL, nullptr);
	IDepthStencilView* pDepthView = GraphicsDevice->CreateDepthStencilView(pTex);
	IRenderTargetInfo* pTarget = GraphicsDevice->CreateRenderPass(pView, pDepthView, float4());

	Scene* Level = GetSceneManager()->CreateNewScene("Test drawing scene");
	Mesh* pMesh = Level->GetMeshCache().LoadMesh("Meshes/TestMesh.fbx");

	IGraphicsPipelineState* DrawingPipeline = GetShaderCache()->LoadGraphicsPipeline("MainShader.gfx");

	while (!pWindow->QuitRequested())
	{
		IGraphicsCommandContext* Context = GraphicsDevice->GetGraphicsContext();
 		IGraphicsCommandBuffer* CommandBuffer = Context->CreateCommandBuffer();
 		CommandBuffer->BeginPass(BackPass);
		CommandBuffer->SetGraphicsPipelineState(DrawingPipeline);
		CommandBuffer->SetVertexBuffer(pMesh->GetVertexBuffer());
		CommandBuffer->SetIndexBuffer(pMesh->GetIndexBuffer());
 		CommandBuffer->EndPass();
 		CommandBuffer->CloseCommandBuffer();
 		Context->ExecuteCommandBuffer(CommandBuffer);
		GraphicsDevice->GetSwapChain()->Present();
	}
}


void InitApplication()
{
	GetSceneManager()->CreateNewScene("Test scene");
	Scene* Level = GetSceneManager()->CurrentScene;
	{
		Mesh* AllocMesh = Level->GetMeshCache().AllocateMesh();
		AllocMesh->GenerateUVSphere(32);
		Level->GetMeshCache().SaveMesh(AllocMesh, "Sphere");

		AllocMesh = Level->GetMeshCache().AllocateMesh();
		AllocMesh->GenerateCylinder(32, 3);
		Level->GetMeshCache().SaveMesh(AllocMesh, "Cylinder");
	}

	Entity* CameraEntity = Level->CreateEntity("Camera");
	auto CamComp = CameraEntity->AddComponent<CameraComponent>();
	CamComp->ZNear = 1e-2f;
	CamComp->ZFar = 1e+21f;
	CameraEntity->AddComponent<CameraMovementComponent>()->Speed = 1.;
	CameraEntity->AddComponent<RaycastTestComponent>();
	CameraEntity->AddComponent<TestUIComponent>();

	Entity* Test = Level->CreateEntity("TestEntity");
	Test->AddComponent<TestRenderingMeshComponent>();

#if 0
	{
		Entity* CylinderMeshTest = Level->CreateEntity("Collision test");
		auto MCC = CylinderMeshTest->AddComponent<MeshRenderingComponent>();
		MCC->RenderedMesh = Level->GetMeshCache().LoadMesh("Sphere");
		MCC->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MCC->RenderedMaterial.SetDiffuseTexture("Resources/8k_moon.jpg");
		auto CCC = CylinderMeshTest->AddComponent<SphereCollisionComponent>();
		CCC->SetMass(10.0);
		CCC->SetPosition(double3(0., -10., 0.));
		//CCC->SetVelocity(double3(0., 1. / 120., -1e-10));
		CCC->SetRadius(1.0);
	}
	{
		Entity* AnotherMeshTest = Level->CreateEntity("Another Collision Test");
		auto MCC = AnotherMeshTest->AddComponent<MeshRenderingComponent>();
		MCC->RenderedMesh = Level->GetMeshCache().LoadMesh("Sphere");
		MCC->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MCC->RenderedMaterial.SetDiffuseTexture("DefaultBlue");
		auto CCC = AnotherMeshTest->AddComponent<SphereCollisionComponent>();
		CCC->SetMass(10000000.0);
		CCC->SetPosition(double3(-10., 0., 0.));
		//CCC->SetVelocity(double3(1. / 120., 0., 0.));
		CCC->SetRadius(1.0);
	}
	{
		Entity* AnotherMeshTest = Level->CreateEntity("Another Collision Test1");
		auto MCC = AnotherMeshTest->AddComponent<MeshRenderingComponent>();
		MCC->RenderedMesh = Level->GetMeshCache().LoadMesh("Sphere");
		MCC->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MCC->RenderedMaterial.SetDiffuseTexture("DefaultBlue");
		auto CCC = AnotherMeshTest->AddComponent<SphereCollisionComponent>();
		CCC->SetMass(1000000000.0);
		CCC->SetPosition(double3(-10., 03., 0.));
		//CCC->SetVelocity(double3(1. / 120., 0., 0.));
		CCC->SetRadius(1.0);
	}
	{
		Entity* AnotherMeshTest = Level->CreateEntity("Another Collision Test2");
		auto MCC = AnotherMeshTest->AddComponent<MeshRenderingComponent>();
		MCC->RenderedMesh = Level->GetMeshCache().LoadMesh("Sphere");
		MCC->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MCC->RenderedMaterial.SetDiffuseTexture("DefaultBlue");
		auto CCC = AnotherMeshTest->AddComponent<SphereCollisionComponent>();
		CCC->SetMass(1000000000.0);
		CCC->SetPosition(double3(-10., 0., 014.));
		CCC->SetVelocity(-double3(1. / 120., 0., 0.));
		CCC->SetRadius(1.0);
	}
	{
		Entity* AnotherMeshTest = Level->CreateEntity("Another Collision Test3");
		auto MCC = AnotherMeshTest->AddComponent<MeshRenderingComponent>();
		MCC->RenderedMesh = Level->GetMeshCache().LoadMesh("Sphere");
		MCC->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MCC->RenderedMaterial.SetDiffuseTexture("DefaultBlue");
		auto CCC = AnotherMeshTest->AddComponent<SphereCollisionComponent>();
		CCC->SetMass(10000000.0);
		CCC->SetPosition(double3(-17., 0., 0.));
		CCC->SetVelocity(double3(1. / 120., 0., 0.));
		CCC->SetRadius(1.0);
	}
#endif
}

void InitApplication1()
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
		auto SunMesh = Sun->AddComponent<MeshRenderingComponent>();
		SunMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		SunMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		SunMesh->RenderedMaterial.SetTexture("Resources/8k_sun.jpg", 0);
		Sun->GetTransform()->Scale(1391016000.0);
		auto SCC = Sun->AddComponent<SphereCollisionComponent>();
		SCC->SetPosition(SInfo.Position);
		SCC->SetMass(1.989e30);
		SCC->SetRadius(13910160000.0 / 2.);
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
		//Mercury->GetPhysicsProperties().bCanTick = false;
		Mercury->GetTransform()->SetPosition(SInfo.Position);
		auto MercuryMesh = Mercury->AddComponent<MeshRenderingComponent>();
		MercuryMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		MercuryMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		MercuryMesh->RenderedMaterial.SetTexture("Resources/2k_mercury.jpg", 0);
		Mercury->GetTransform()->Scale(4879000.0);
		auto SCC = Mercury->AddComponent<SphereCollisionComponent>();
		SCC->SetMass(3.285e23);
		SCC->SetVelocity(SInfo.Velocity * (1.0 / 60.));
		SCC->SetPosition(SInfo.Position);
		SCC->SetRadius(4879000.0 / 2.);
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
		Venus->GetTransform()->SetPosition(SInfo.Position);
		auto VenusMesh = Venus->AddComponent<MeshRenderingComponent>();
		VenusMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		VenusMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		VenusMesh->RenderedMaterial.SetTexture("Resources/8k_venus_atmosphere.jpg", 0);
		Venus->GetTransform()->Scale(12104000.);
		auto SCC = Venus->AddComponent<SphereCollisionComponent>();
		SCC->SetPosition(SInfo.Position);
		SCC->SetVelocity(SInfo.Velocity * 1.0 / 60.0);
		SCC->SetMass(4.867e24);
		SCC->SetRadius(12104000. / 2.);
	}
	{
		Entity* Earth = SpaceLevel->CreateEntity("Earth");
		PLANET_START_INFO SInfo = PlacePlanet({ "Earth", 0., 0., 0., M_AU(1.0), 0., 0. }, TimeArg);
		std::cout << "Earth Distance: " << length(SInfo.Position) << std::endl;
		auto EarthMesh = Earth->AddComponent<MeshRenderingComponent>();
		EarthMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		EarthMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		EarthMesh->RenderedMaterial.SetTexture("Resources/EarthNoClouds.jpg", 0);
		Earth->GetTransform()->Scale(12742000.0);
		auto SCC = Earth->AddComponent<SphereCollisionComponent>();
		SCC->SetMass(5.97219e24);
		SCC->SetVelocity(SInfo.Velocity * 1.0 / 60.);
		SCC->SetPosition(SInfo.Position);
		SCC->SetRadius(12742000.0 / 2.);
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
		auto LunaMesh = Luna->AddComponent<MeshRenderingComponent>();
		LunaMesh->RenderedMesh = SpaceLevel->GetMeshCache().LoadMesh("Sphere");
		LunaMesh->RenderedMaterial.InitializeMaterial("MainShader.gfx");
		LunaMesh->RenderedMaterial.SetTexture("Resources/8k_moon.jpg", 0);
		Luna->GetTransform()->Scale(3474000.0);
		auto SCC = Luna->AddComponent<SphereCollisionComponent>();
		SCC->SetMass(7.34767309e22);
		SCC->SetVelocity(SInfo.Velocity * (1. / 60.));
		SCC->SetPosition(SInfo.Position);
		SCC->SetRadius(3474000.0 / 2.);
	}

	Entity* CamEntity = SpaceLevel->CreateEntity("Camera");
	CamEntity->GetTransform()->SetPosition(FollowedPlanet->GetTransform()->GetPosition());
	CamEntity->GetTransform()->SetRotation(float3(0.f, 0.f, 0.f));
	auto Cam = CamEntity->AddComponent<CameraComponent>();
	Cam->ZNear = 1e-2f;
	Cam->ZFar = 1e+21f; 
	CamEntity->AddComponent<CameraMovementComponent>()->Speed = 1e12;
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



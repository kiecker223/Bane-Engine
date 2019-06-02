#include "Application.h"
#include "Rendering/RendererInterface.h"
#include "Rendering/BasicForwardRenderer.h"
#include "GraphicsCore/IO/TextureCache.h"
#include "GraphicsCore/IO/ShaderCache.h"
#include "GameObjects/PhysicsData.h"
#include <Platform/System/Logging/Logger.h>
#include <Core/Data/Timer.h>
#include <random>
#include <Platform/System/Process.h>
#include "../GameObjects/NBodyAcceleration.h"
#include <Platform/Input/InputSystem.h>
#include <iostream>
#include <sys/timeb.h>
#include <sys/utime.h>
#include "../GameObjects/PhysicsObjectPools.h"
#include <sstream>
#include "../Game/PlanetInfo.h"

#pragma warning(disable:4049)

#define NUM_OBJECTS 5


Application* Application::GApplication = nullptr;

Application::Application() :
	m_SceneRenderer(nullptr)
{
	
}

Application::Application(int argc, char** argv) :
	m_SceneRenderer(nullptr)
{
	UNUSED(argc);
	UNUSED(argv);
}

Application::~Application()
{
}

bool Application::RegisterApplication(Application* App)
{
	if (App)
	{
		if (GApplication)
		{
			// Log that we already have an active application
			return false;
		}
		GApplication = App; // We can assume that its already initialized and all that jazz
	}

	return true;
}

void Application::OpenApplicationWindow(const std::string& Name, uint32 Width, uint32 Height)
{
	if (m_Window)
	{
		delete m_Window;
	}
	Window* pWindow = new Window(Name.c_str(), Width, Height);
	Window::SetMainWindow(pWindow);
	m_Window = pWindow;
}

void Application::InitSystems()
{
	if (!RegisterApplication(this))
	{
		// Error!
	}
	GlobalLog::Initialize();

	m_TaskSystem = new TaskSystem();
	m_TaskSystem->Initialize();
	if (!m_Window)
	{
		uint32 ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
		uint32 ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		//ScreenWidth = uint32(ScreenWidth *  (float)0.9);
		//ScreenHeight = uint32(ScreenHeight * (float)0.9);
		OpenApplicationWindow("Unnamed window", ScreenWidth, ScreenHeight);
	}
	InitializeInput(m_Window);
	ApiRuntime::CreateRuntime();
	GetApiRuntime()->Initialize(m_Window);

	char str[256];
	GetCurrentDirectoryA(256, str);
	UNUSED(str);

 	IProcessHandle* Handle = StartProcess("BaneShaderCompiler.exe", { "Dx12", "Shaders/", "CompiledShaders/" });
 	Handle->WaitForFinish();
 	
	InitShaderCache("CompiledShaders/ShaderPipelines.json");
	InitializeTextureCache();

	if (m_SceneRenderer == nullptr)
	{
		m_SceneRenderer = new BasicForwardRenderer();
	}
	m_SceneRenderer->Initialize(m_Window);
	
	CollisionShapeObjectPool::Initialize();
	PhysicsDataObjectPool::Initialize();
}

void ResetVelocities(std::vector<CurrentPhysicsData>& InPhysicsData)
{
	for (uint32 i = 0; i < InPhysicsData.size(); i++)
	{
		InPhysicsData[i].Velocity = vec3();
	}
}

void NBodyCalculation(uint32 StartIndex, uint32 EndIndex, std::vector<PhysicsData>& InOutPhysicsData)
{
	for (uint32 x = StartIndex; x < EndIndex; x++)
	{
		for (uint32 i = 0; i < InOutPhysicsData.size(); i++)
		{
			if (i == x) { continue; }
			vec3 ForceDir = InOutPhysicsData[i].Position - InOutPhysicsData[x].Position;
			double DistanceSqrd = lengthSqrd(ForceDir);
			normalize(ForceDir);
			double Force = M_GRAV_CONST * (100000.0 * 100000.0) / (DistanceSqrd);
			if (isnan(Force)) { continue; }
			InOutPhysicsData[x].Velocity += ForceDir * Force * (1.0 / 60.0);
		}
	}
}

void Application::Run()
{
	Timer FrameTime;
	bool bLimitFrames = false;
	double DT = 0.;
	
	// Setup data needed for Tasks
	std::vector<CurrentPhysicsData> PhysicsData;
	PhysicsData.resize(NUM_OBJECTS);

	{
		const double TimeArg = CalculateTimeForGame(200.0, 3.0, 14.0, 2.0, 0.0, 0.0);
		const double FTimeArg = CalculateTimeForGame(200.0, 3.0, 14.0, 2.0, 0.0, 1.0);
		vec3 FEarthInfo = -GetPlanetPosition(
				{
					"Sun",
					0.0,
					0.0,
					282.9404 + 4.70935E-5 * FTimeArg,
					M_AU(1.0),
					0.016709 - 1.151E-9*FTimeArg,
					356.0470 + 0.9856002585 * FTimeArg
				});

		vec3 EarthInfo = -GetPlanetPosition(
			{
				"Sun",
				0.0,
				0.0,
				282.9404 + 4.70935E-5 * TimeArg,
				M_AU(1.0),
				0.016709 - 1.151E-9*TimeArg,
				356.0470 + 0.9856002585 * TimeArg
			});

		{
			PhysicsData[0].Position = vec3(0.0, 0.0, 0.0);
			PhysicsData[0].Mass = 1.989e30;
		}
		{
			vec3 SInfo = GetPlanetPosition(
				{
					"Mercury",
					48.3313 + 3.24587E-5 * TimeArg,
					7.0047 + 5.00E-8 * TimeArg,
					29.1241 + 1.01444E-5 * TimeArg,
					M_AU(0.397098),
					0.205635 + 5.59E-10 * TimeArg,
					168.6562 + 4.0923344368 * TimeArg
				}
			);

			vec3 FInfo = GetPlanetPosition(
				{
					"Mercury",
					48.3313 + 3.24587E-5 * FTimeArg,
					7.0047 + 5.00E-8 * FTimeArg,
					29.1241 + 1.01444E-5 * FTimeArg,
					M_AU(0.397098),
					0.205635 + 5.59E-10 * FTimeArg,
					168.6562 + 4.0923344368 * FTimeArg
				}
			);

			PhysicsData[1].Position = SInfo;
			//PhysicsData[1].Velocity = FInfo - SInfo;
			PhysicsData[1].Mass = 3.285e23;
		}
		{
			vec3 SInfo = GetPlanetPosition(
				{
					"Venus",
					76.6799 + 2.46590E-5 * TimeArg,
					3.3946 + 2.75E-8 * TimeArg,
					54.8910 + 1.38374E-5 * TimeArg,
					M_AU(0.723330),
					0.006773 - 1.302E-9 * TimeArg,
					48.0052 + 1.6021302244 * TimeArg
				}
			);

			vec3 FInfo = GetPlanetPosition(
				{
					"Venus",
					76.6799 + 2.46590E-5 * FTimeArg,
					3.3946 + 2.75E-8 * FTimeArg,
					54.8910 + 1.38374E-5 * FTimeArg,
					M_AU(0.723330),
					0.006773 - 1.302E-9 * FTimeArg,
					48.0052 + 1.6021302244 * FTimeArg
				}
			);

			PhysicsData[2].Position = SInfo;
			//PhysicsData[2].Velocity = FInfo - SInfo;
			PhysicsData[2].Mass = 4.867e24;
		}
		{
			PhysicsData[3].Position = EarthInfo;
			//PhysicsData[3].Velocity = FEarthInfo - EarthInfo;
			PhysicsData[3].Mass = 5.97219e24;
		}
		{
			vec3 SInfo = GetPlanetPosition(
				{
					"Moon",
					125.1228 - 0.0529538083 * TimeArg,
					5.1454,
					318.0634 + 0.1643573223 * TimeArg,
					60.266 * (12742000.0 / 2.),
					0.054900,
					1163654 + 13.069929509 * TimeArg
				}
			);

			vec3 FInfo = GetPlanetPosition(
				{
					"Moon",
					125.1228 - 0.0529538083 * FTimeArg,
					5.1454,
					318.0634 + 0.1643573223 * FTimeArg,
					60.266 * (12742000.0 / 2.),
					0.054900,
					1163654 + 13.069929509 * FTimeArg
				}
			);

			PhysicsData[4].Position = EarthInfo + SInfo;
			//PhysicsData[4].Velocity = (FEarthInfo + FInfo) - (EarthInfo + SInfo);
			PhysicsData[4].Mass = 7.34767309e22;
		}
	}

	for (uint32 i = 0; i < PhysicsData.size(); i++)
	{
		std::cout << PhysicsData[i].Position.x << " " << PhysicsData[i].Position.y << " " << PhysicsData[i].Position.z << std::endl;
		std::cout << length(PhysicsData[i].Velocity) / 1000.0 << " KM/s" << std::endl;
	}
	
	// Create the shared resource handle
	
	Task* CalculateNBodyAccelerationTask = new Task(TASK_DISPATCH_ON_ALL_THREADS, [&PhysicsData](uint32 DispatchSize, uint32 DispatchIndex) 
	{
		uint32 NumToIterate = NUM_OBJECTS / DispatchSize;
		uint32 StartIndex = NumToIterate * DispatchIndex;
		uint32 EndIndex = StartIndex + NumToIterate;
		NBodyCalculation(StartIndex, EndIndex, PhysicsData);
	});

	IGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	ITexture2D* Tex = GetTextureCache()->LoadTexture("DefaultBlue");
	IGraphicsPipelineState* PipelineState = GetShaderCache()->LoadGraphicsPipeline("MainShader.gfx");
	Mesh* pMesh = new Mesh();
	pMesh->GenerateUVSphere(12);

	IBuffer* CameraBuff = Device->CreateConstBuffer<matrix>();
	IBuffer* ConstantBuff = Device->CreateConstantBuffer(256 * 12000);

	struct ConstantBuffMappedPointerRef
	{
		struct ALIGN_FOR_GPU_BUFFER Reference
		{
			matrix Mat;
		} *pPointer;
	} ConstantBuffMappedRef;


	vec3 CameraPosition = PhysicsData[3].Position;

	Task* ApplyNBodyAccelerationTask = new Task(TASK_DISPATCH_ON_ALL_THREADS, [&PhysicsData, &ConstantBuffMappedRef, &CameraPosition](uint32 DispatchSize, uint32 DispatchIndex)
	{
		uint32 NumToIterate = NUM_OBJECTS / DispatchSize;
		uint32 StartIndex = NumToIterate * DispatchIndex;
		uint32 EndIndex = StartIndex + NumToIterate;
		for (uint32 x = StartIndex; x < EndIndex; x++)
		{
			PhysicsData[x].Position += PhysicsData[x].Velocity;
			if (isNan(PhysicsData[x].Position)) { __debugbreak(); }
		}
	});

	Task* MoveShitRight = new Task(3, [&PhysicsData, &ConstantBuffMappedRef](uint32 DispatchSize, uint32 DispatchIndex)
	{
		uint32 NumToIterate = NUM_OBJECTS / 6;
		uint32 StartIndex = NumToIterate * DispatchIndex;
		uint32 EndIndex = StartIndex + NumToIterate;
		for (uint32 x = StartIndex; x < EndIndex; x++)
		{
			PhysicsData[x].Position += vec3(0.001, 0.0, 0.0);
		}
	});

	Task* MoveShitLeft = new Task(3, [&PhysicsData, &ConstantBuffMappedRef, &MoveShitRight](uint32 DispatchSize, uint32 DispatchIndex)
	{
		uint32 NumToIterate = NUM_OBJECTS / 6;
		uint32 StartIndex = NumToIterate * (DispatchIndex + 3);
		uint32 EndIndex = StartIndex + NumToIterate;
		for (uint32 x = StartIndex; x < EndIndex; x++)
		{
			PhysicsData[x].Position += vec3(-0.001, 0.0, 0.0);
		}
		if (DispatchIndex == 2)
		{
			Dispatcher::DispatchTask(MoveShitRight);
		}
	});

	Task* ApplyTransforms = new Task(TASK_DISPATCH_ON_ALL_THREADS, [&PhysicsData, &ConstantBuffMappedRef](uint32 DispatchSize, uint32 DispatchIndex)
	{
		uint32 NumToIterate = NUM_OBJECTS / DispatchSize;
		uint32 StartIndex = NumToIterate * DispatchIndex;
		uint32 EndIndex = StartIndex + NumToIterate;
		for (uint32 x = StartIndex; x < EndIndex; x++)
		{
			//ConstantBuffMappedRef.pPointer[x] = matTranslation(fromDouble3(PhysicsData[x].Position)) * matScale(fvec3(3000.0f, 3000.0f, 3000.0f));
		}
	});

	NBodyAcceleration AccelerationStructure;

	double MovementSpeed = 1.0;

	Quaternion LookDirection;
	SIMDMEMZERO(CameraBuff->Map(), CameraBuff->GetSizeInBytes());

	while (!m_Window->QuitRequested())
	{
		// Allow for moving around
		ConstantBuffMappedRef.pPointer = ConstantBuff->MapT<ConstantBuffMappedPointerRef::Reference>();
		SIMDMEMZERO(ConstantBuffMappedRef.pPointer, ConstantBuff->GetSizeInBytes());
		GetInput()->Update();
		if (GetInput()->Keyboard.GetKey(KEY_S))
		{
			CameraPosition.z -= MovementSpeed;
		}
		if (GetInput()->Keyboard.GetKey(KEY_W))
		{
			CameraPosition.z += MovementSpeed;
		}
		if (GetInput()->Keyboard.GetKey(KEY_A))
		{
			CameraPosition.x += MovementSpeed;
		}
		if (GetInput()->Keyboard.GetKey(KEY_D))
		{
			CameraPosition.x -= MovementSpeed;
		}
		if (GetInput()->Mouse.GetScrollWheel() > 0.0f)
		{
			MovementSpeed *= 10.0f;
		}
		if (GetInput()->Mouse.GetScrollWheel() < 0.0f)
		{
			MovementSpeed /= 10.0f;
		}
		if (GetInput()->Keyboard.GetKeyDown(KEY_SPACE))
		{
			ResetVelocities(PhysicsData);
		}

		vec2 Diff = GetInput()->Mouse.GetMouseDelta();
		LookDirection *= Quaternion::FromAxisAngle(vec3(1.0, 0.0, 0.0), Diff.y / 100.0);
		LookDirection *= Quaternion::FromAxisAngle(vec3(0.0, 1.0, 0.0), Diff.x / 100.0);
		LookDirection.Normalize();

		// Schedule the tasks to be executed
		AccelerationStructure.Construct(PhysicsData);
		AccelerationStructure.Simulate(PhysicsData, 1.0 / 60.0);

		/*CameraPosition = PhysicsData[3].Position + vec3(0.0, 0.0, 100.0);*/

		//Dispatcher::DispatchTask(CalculateNBodyAccelerationTask);
		Dispatcher::DispatchTask(ApplyNBodyAccelerationTask);
		Dispatcher::WaitOnTask(ApplyNBodyAccelerationTask);

		for (uint32 i = 0; i < PhysicsData.size(); i++)
		{
			vec3 TransformedPosition = PhysicsData[i].Position - CameraPosition;
			if (isNan(TransformedPosition)) { __debugbreak(); }
			matrix Mat = matTranslation(fromDouble3(TransformedPosition)) * matScale(fvec3(3000000.0f, 3000000.0f, 3000000.0f));
			ConstantBuffMappedRef.pPointer[i].Mat = Mat;
		}

		// Setup the constant buffer data
		matrix* pMats = CameraBuff->MapT<matrix>();
		auto Mats = matProjection(m_Window->AspectXY(), 60.f, 0.001f, 1e+23f) * matView(fvec3(0.0, 0.0, 0.0), LookDirection.RotationMatrix3x3() * fvec3(0.0f, 0.0f, 1.0f), fvec3(0.0f, 1.0f, 0.0f));
		*pMats = Mats;
		// Draw the shit
		IGraphicsCommandContext* pCtx = Device->GetGraphicsContext();
		pCtx->BeginPass(GetApiRuntime()->GetGraphicsDevice()->GetBackBufferTargetPass());
		pCtx->SetConstantBuffer(0, CameraBuff);
		pCtx->SetGraphicsPipelineState(PipelineState);
		pCtx->SetTexture(0, Tex);
		pCtx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCtx->SetVertexBuffer(pMesh->GetVertexBuffer());
		pCtx->SetIndexBuffer(pMesh->GetIndexBuffer());
		for (uint32 i = 0; i < PhysicsData.size(); i++)
		{
			pCtx->SetConstantBuffer(1, ConstantBuff, 256 * i);
			pCtx->DrawIndexed(pMesh->GetIndexCount(), 0, 0);
		}
		pCtx->EndPass();

		m_SceneRenderer->Present();
		FrameTime.StartTimer();
	}
	m_TaskSystem->Shutdown();
}

void Application::Shutdown()
{
	DestroyShaderCache();
	DestroyTextureCache();
	m_SceneRenderer->Shutdown();
	delete m_SceneRenderer;
	GlobalLog::Cleanup();
}


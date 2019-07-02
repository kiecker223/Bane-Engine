#include "Application.h"
#include "Rendering/RendererInterface.h"
#include "Rendering/BasicForwardRenderer.h"
#include "GraphicsCore/IO/TextureCache.h"
#include "GraphicsCore/IO/ShaderCache.h"
#include "Physics/NBodyAcceleration.h"
#include <Platform/System/Logging/Logger.h>
#include <Core/Data/Timer.h>
#include <random>
#include <Platform/System/Process.h>
#include "../Physics/NBodyAcceleration.h"
#include <Platform/Input/InputSystem.h>
#include <iostream>
#include <sys/timeb.h>
#include <sys/utime.h>
#include "../Physics/PhysicsObjectPools.h"
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

	for (uint32 i = 0; i < PhysicsData.size(); i++)
	{
		vec3 RandomPos = RandomVec3();
		PhysicsData[i].Position = RandomPos;
		PhysicsData[i].Mass = 2000000.0;
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

	Quaternion LookDirection(fvec3(0.0f, 0.0f, 0.0f));
	SIMDMEMZERO(CameraBuff->Map(), CameraBuff->GetSizeInBytes());

	while (!m_Window->QuitRequested())
	{
		// Allow for moving around
		ConstantBuffMappedRef.pPointer = ConstantBuff->MapT<ConstantBuffMappedPointerRef::Reference>();
		SIMDMEMZERO(ConstantBuffMappedRef.pPointer, ConstantBuff->GetSizeInBytes());
		GetInput()->Update();
		vec3 Velocity;
		if (GetInput()->Keyboard.GetKey(KEY_S))
		{
			Velocity.z -= MovementSpeed;
		}
		if (GetInput()->Keyboard.GetKey(KEY_W))
		{
			Velocity.z += MovementSpeed;
		}
		if (GetInput()->Keyboard.GetKey(KEY_A))
		{
			Velocity.x -= MovementSpeed;
		}
		if (GetInput()->Keyboard.GetKey(KEY_D))
		{
			Velocity.x += MovementSpeed;
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
		if (abs(Diff.y) > 0.0)
		{
			vec3 Right = fromFloat3(LookDirection.RotationMatrix3x3() * fvec3(1.0f, 0.0f, 0.0f));
			LookDirection *= Quaternion::FromAxisAngle(Right, Diff.y / 1000.0);
		}
		if (abs(Diff.x) > 0.0)
		{
			LookDirection *= Quaternion::FromAxisAngle(vec3(0.0, 1.0, 0.0), Diff.x / 1000.0);
		}

		//Velocity = LookDirection.RotationMatrix3x3Doubles() * Velocity;
		CameraPosition += Velocity;
		// Schedule the tasks to be executed
		//AccelerationStructure.Construct(PhysicsData);
		//AccelerationStructure.Simulate(PhysicsData, 1.0 / 60.0);

// 		for (uint32 i = 0; i < PhysicsData.size(); i++)
// 		{
// 			PhysicsData[i].Velocity = vec3(0.5, 0.0, 0.5);
// 		}

		/*CameraPosition = PhysicsData[3].Position + vec3(0.0, 0.0, 100.0);*/

		Dispatcher::DispatchTask(ApplyNBodyAccelerationTask);
		Dispatcher::WaitOnTask(ApplyNBodyAccelerationTask);

		for (uint32 i = 0; i < PhysicsData.size(); i++)
		{
			vec3 TransformedPosition = PhysicsData[i].Position - CameraPosition;
			if (isNan(TransformedPosition)) { __debugbreak(); }
			matrix Mat = matTranslation(fromDouble3(TransformedPosition)) * matRotation(LookDirection) * matScale(fvec3(300.0f, 300.0f, 300.0f));
			ConstantBuffMappedRef.pPointer[i].Mat = Mat;
		}

		// Setup the constant buffer data
		matrix* pMats = CameraBuff->MapT<matrix>();
		//*pMats = matView(fvec3(0.0, 0.0, 0.0), fvec3(0.0f, 0.0f, 1.0f), fvec3(0.0f, 1.0f, 0.0f)) * matProjection(m_Window->AspectXY(), 60.f, 0.001f, 1e+23f);
		//*pMats = matProjection(m_Window->AspectXY(), 60.f, 0.001f, 1e+23f) * matView(fvec3(0.0, 0.0, 0.0), fvec3(0.0f, 0.0f, 1.0f), fvec3(0.0f, 1.0f, 0.0f));
		*pMats = matrix(1.0f) * matProjection(m_Window->AspectXY(), 60.f, 0.001f, 1e+23f);
		//pMats[1] = matView(fvec3(0.0, 0.0, 0.0), fvec3(0.0f, 0.0f, 1.0f), fvec3(0.0f, 1.0f, 0.0f));

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


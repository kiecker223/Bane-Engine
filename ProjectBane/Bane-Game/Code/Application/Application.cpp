#include "Application.h"
#include "Rendering/RendererInterface.h"
#include "Rendering/BasicForwardRenderer.h"
#include "GraphicsCore/IO/TextureCache.h"
#include "GraphicsCore/IO/ShaderCache.h"
#include "GameObjects/PhysicsData.h"
#include <Platform/System/Logging/Logger.h>
#include <Core/Data/Timer.h>
#include <Platform/System/Process.h>
#include <Platform/Input/InputSystem.h>
#include <iostream>
#include <sys/timeb.h>
#include <sys/utime.h>
#include <sstream>

#pragma warning(disable:4049)

#define NUM_OBJECTS 96


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
}



void Application::Run()
{
	Timer FrameTime;
	bool bLimitFrames = false;
	double DT = 0.;
	
	// Setup data needed for Tasks
	std::vector<CurrentPhysicsData> PhysicsData;
	PhysicsData.resize(NUM_OBJECTS);
	for (uint32 i = 0; i < NUM_OBJECTS; i++)
	{
		PhysicsData[i].Position = 
			fromFloat3(Quaternion::FromAxisAngle(fvec3(0.0f, 1.0f, 0.0f), (static_cast<float>(i) / static_cast<float>(NUM_OBJECTS)) * 2.0f * _PI_) * fvec3(0.0, 0.0, 100.0));
	}

	// Create the shared resource handle
	
	Task* CalculateNBodyAccelerationTask = new Task(TASK_DISPATCH_ON_ALL_THREADS, [&PhysicsData](uint32 DispatchSize, uint32 DispatchIndex) 
	{
		uint32 NumToIterate = NUM_OBJECTS / DispatchSize;
		uint32 StartIndex = NumToIterate * DispatchIndex;
		uint32 EndIndex = StartIndex + NumToIterate;
		for (uint32 x = StartIndex; x < EndIndex; x++)
		{
			for (uint32 i = 0; i < PhysicsData.size(); i++)
			{
				if (i == x) { continue; }
				vec3 ForceDir = PhysicsData[i].Position - PhysicsData[x].Position;
				double Distance = length(ForceDir);
				normalize(ForceDir);
				double Force = M_GRAV_CONST * (100000.0 * 100000.0) / (Distance * Distance);
				if (isnan(Force)) { continue; }
				PhysicsData[x].Velocity += ForceDir * Force * (1.0 / 60.0);
			}
		}
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
		matrix* pPointer;
	} ConstantBuffMappedRef;

	Task* ApplyNBodyAccelerationTask = new Task(TASK_DISPATCH_ON_ALL_THREADS, [&PhysicsData, &ConstantBuffMappedRef](uint32 DispatchSize, uint32 DispatchIndex)
	{
		uint32 NumToIterate = NUM_OBJECTS / DispatchSize;
		uint32 StartIndex = NumToIterate * DispatchIndex;
		uint32 EndIndex = StartIndex + NumToIterate;
		for (uint32 x = StartIndex; x < EndIndex; x++)
		{
			PhysicsData[x].Position += PhysicsData[x].Velocity;
			ConstantBuffMappedRef.pPointer[x] = matTranslation(fromDouble3(PhysicsData[x].Position)) * matScale(fvec3(30.0f, 30.0f, 30.0f));
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
			ConstantBuffMappedRef.pPointer[x] = matTranslation(fromDouble3(PhysicsData[x].Position)) * matScale(fvec3(30.0f, 30.0f, 30.0f));
		}
	});

	vec3 CameraPosition;

	while (!m_Window->QuitRequested())
	{
		// Allow for moving around
		ConstantBuffMappedRef.pPointer = ConstantBuff->MapT<matrix>();
		GetInput()->Update();
		if (GetInput()->Keyboard.GetKey(KEY_S))
		{
			CameraPosition.z -= 1.0f;
		}
		if (GetInput()->Keyboard.GetKey(KEY_W))
		{
			CameraPosition.z += 1.0f;
		}
		if (GetInput()->Keyboard.GetKey(KEY_A))
		{
			CameraPosition.y += 1.0f;
		}
		if (GetInput()->Keyboard.GetKey(KEY_D))
		{
			CameraPosition.y -= 1.0f;
		}

		// Schedule the tasks to be executed
		Dispatcher::Begin();
		Dispatcher::DispatchTasks({ MoveShitLeft });
		Dispatcher::DispatchTask(ApplyTransforms);
		Dispatcher::End();
		Dispatcher::WaitOnTask(ApplyTransforms);

		// Setup the constant buffer data
		matrix* pMats = CameraBuff->MapT<matrix>();
		*pMats = matProjection(1.33f, 60.f, 0.001f, 10000000.0f) * matView(fromDouble3(CameraPosition), fvec3(0.0f, 0.0f, 0.0f), fvec3(0.0f, 1.0f, 0.0f));

		// Draw the shit
		IGraphicsCommandContext* pCtx = Device->GetGraphicsContext();
		pCtx->BeginPass(GetApiRuntime()->GetGraphicsDevice()->GetBackBufferTargetPass());
		pCtx->SetConstantBuffer(0, CameraBuff);
		pCtx->SetGraphicsPipelineState(PipelineState);
		pCtx->SetTexture(0, Tex);
		for (uint32 i = 0; i < PhysicsData.size(); i++)
		{
			pCtx->SetConstantBuffer(1, ConstantBuff, 256 * i);
			pCtx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pCtx->SetVertexBuffer(pMesh->GetVertexBuffer());
			pCtx->SetIndexBuffer(pMesh->GetIndexBuffer());
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


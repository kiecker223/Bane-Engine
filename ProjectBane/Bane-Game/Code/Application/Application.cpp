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

#define NUM_OBJECTS 100

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

class UpdatePhysicsBodyTask : public Task
{
public:

	UpdatePhysicsBodyTask(SharedTaskResource& InDependentResource, int32 InThreadCount, uint32 InSizeOfForeach, ETASK_EXECUTION_TYPE InExecutionType) :
		Task(InDependentResource, InThreadCount, InSizeOfForeach, InExecutionType)
	{
	}

	class UpdatePhysicsTaskExecutionHandle : public ITaskExecutionHandle
	{
	public:

		std::vector<CurrentPhysicsData>& Data;
		UpdatePhysicsTaskExecutionHandle(std::vector<CurrentPhysicsData>& InData) : Data(InData) { }

		uint32 StartIndex, EndIndex;
		bool bIsFinished = false;
		bool bStarted = false;

		// Calculate the bodies velocities
		void Execute()
		{
			bStarted = true;
			bIsFinished = false;
			for (uint32 x = StartIndex; x < EndIndex; x++)
			{
				for (uint32 i = 0; i < Data.size(); i++)
				{
					if (i == x) { continue; }
					vec3 ForceDir = Data[i].Position - Data[x].Position;
					double Distance = length(ForceDir);
					normalize(ForceDir);
					double Force = M_GRAV_CONST * (100000.0 * 100000.0) / (Distance * Distance);
					if (isnan(Force)) { continue; }
					Data[x].Velocity += ForceDir * Force * (1.0 / 60.0);
				}
			}
			bIsFinished = true;
		}

		bool IsFinished() const override
		{
			return bIsFinished;
		}

	};
	
	ITaskExecutionHandle* CreateTaskExecutionHandle(uint32 WorkerThreadIndex)
	{
		if (m_TaskExecutionHandles.size() >= WorkerThreadIndex + 1)
		{
			if (m_TaskExecutionHandles[WorkerThreadIndex] != nullptr)
			{
				return m_TaskExecutionHandles[WorkerThreadIndex];
			}
		}
		if (m_TaskExecutionHandles.size() != ThreadDispatchCount)
		{
			m_TaskExecutionHandles.resize(ThreadDispatchCount);
		}
		auto* PhysicsData = DependentResource.ResourceHandle.Get<std::vector<CurrentPhysicsData>>();
		UpdatePhysicsTaskExecutionHandle* Result = new UpdatePhysicsTaskExecutionHandle(*PhysicsData);
		uint32 NumToIterate = NUM_OBJECTS / ThreadDispatchCount;
		Result->StartIndex = NumToIterate * WorkerThreadIndex;
		Result->EndIndex = (NumToIterate * WorkerThreadIndex) + NumToIterate;
		m_TaskExecutionHandles[WorkerThreadIndex] = Result;
		return Result;
	}

	bool IsFinished() const
	{
		bool bFinished = false;
		for (uint32 i = 0; i < m_TaskExecutionHandles.size(); i++)
		{
			if (m_TaskExecutionHandles[i]->IsFinished() == true)
			{
				bFinished = true;
			}
			else
			{
				bFinished = false;
			}
		}
		return bFinished;
	}
};

class UpdatePhysicsBodyPositionTask : public Task
{
public:
	UpdatePhysicsBodyPositionTask(SharedTaskResource& InDependentResource, int32 InThreadCount, uint32 InSizeOfForeach, ETASK_EXECUTION_TYPE InExecutionType) :
		Task(InDependentResource, InThreadCount, InSizeOfForeach, InExecutionType)
	{
	}

	// The actual execution
	class UpdatePhysicsPositionTaskExecutionHandle : public ITaskExecutionHandle
	{
	public:

		std::vector<CurrentPhysicsData>& Data;
		UpdatePhysicsPositionTaskExecutionHandle(std::vector<CurrentPhysicsData>& InData) : Data(InData) { }

		uint32 StartIndex, EndIndex;
		bool bIsFinished = false;

		void Execute()
		{
			bIsFinished = false;
			for (uint32 x = StartIndex; x < EndIndex; x++)
			{
				Data[x].Position += Data[x].Velocity;
			}
			bIsFinished = true;
		}

		bool IsFinished() const override
		{
			return bIsFinished;
		}

	};

	ITaskExecutionHandle* CreateTaskExecutionHandle(uint32 WorkerThreadIndex)
	{
		// Make sure we aren't just calling new every frame
		if (m_TaskExecutionHandles.size() >= WorkerThreadIndex + 1)
		{
			if (m_TaskExecutionHandles[WorkerThreadIndex] != nullptr)
			{
				return m_TaskExecutionHandles[WorkerThreadIndex];
			}
		}
		if (m_TaskExecutionHandles.size() != ThreadDispatchCount)
		{
			m_TaskExecutionHandles.resize(ThreadDispatchCount);
		}

		// The worker thread index is the thread that the task handle will be executed on
		// So make sure that when the task is dispatched on the thread the data is properly setup
		auto* PhysicsData = DependentResource.ResourceHandle.Get<std::vector<CurrentPhysicsData>>();
		UpdatePhysicsPositionTaskExecutionHandle* Result = new UpdatePhysicsPositionTaskExecutionHandle(*PhysicsData);
		uint32 NumToIterate = NUM_OBJECTS / ThreadDispatchCount;
		Result->StartIndex = NumToIterate * WorkerThreadIndex;
		Result->EndIndex = (NumToIterate * WorkerThreadIndex) + NumToIterate;
		m_TaskExecutionHandles[WorkerThreadIndex] = Result;
		return Result;
	}

	bool IsFinished() const
	{
		bool bFinished = false;
		for (uint32 i = 0; i < m_TaskExecutionHandles.size(); i++)
		{
			if (m_TaskExecutionHandles[i]->IsFinished() == true)
			{
				bFinished = true;
			}
			else
			{
				bFinished = false;
			}
		}
		return bFinished;
	}
};

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
	SharedTaskResource TaskResource;
	TaskResource.ResourceHandle.Set<std::vector<CurrentPhysicsData>>(&PhysicsData);
	
	// Instantiate the tasks with the task system
	UpdatePhysicsBodyTask* pTask = m_TaskSystem->CreateParallelForTask<UpdatePhysicsBodyTask>(TaskResource, NUM_OBJECTS);
	UpdatePhysicsBodyPositionTask* pNextTask = m_TaskSystem->CreateParallelForTask<UpdatePhysicsBodyPositionTask>(TaskResource, NUM_OBJECTS);

	IGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	ITexture2D* Tex = GetTextureCache()->LoadTexture("DefaultBlue");
	IGraphicsPipelineState* PipelineState = GetShaderCache()->LoadGraphicsPipeline("MainShader.gfx");
	Mesh* pMesh = new Mesh();
	pMesh->GenerateUVSphere(12);

	IBuffer* ConstantBuff = Device->CreateConstantBuffer(256 * 12000);
	IBuffer* CameraBuff = Device->CreateConstBuffer<matrix>();

	vec3 CameraPosition;
	Device->GetSwapChain()->SetSwapInterval(1);

	while (!m_Window->QuitRequested())
	{
		// Allow for moving around
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
 		m_TaskSystem->ScheduleTask(pTask);
 		m_TaskSystem->ScheduleTask(pNextTask);

		// UpdateSchedule is called by main thread to
		// update the work for the worker threads
		// Essentially making sure that the tasks scheduled
		// by ScheduleTask(...) are distributed to their threads
 		m_TaskSystem->UpdateSchedule();

		// Force a flush of the threads
 		m_TaskSystem->WaitForThreadStop();

		// Setup the constant buffer data
		matrix* pMats = CameraBuff->MapT<matrix>();
		*pMats = matProjection(1.33f, 60.f, 0.001f, 10000000.0f) * matView(fromDouble3(CameraPosition), fvec3(0.0f, 0.0f, 0.0f), fvec3(0.0f, 1.0f, 0.0f));
		pMats = ConstantBuff->MapT<matrix>();

		// Draw the shit
		IGraphicsCommandContext* pCtx = Device->GetGraphicsContext();
		pCtx->BeginPass(GetApiRuntime()->GetGraphicsDevice()->GetBackBufferTargetPass());
		pCtx->SetConstantBuffer(0, CameraBuff);
		pCtx->SetGraphicsPipelineState(PipelineState);
		pCtx->SetTexture(0, Tex);
		for (uint32 i = 0; i < PhysicsData.size(); i++)
		{
			pMats[i] = matTranslation(fromDouble3(PhysicsData[i].Position)) * matScale(fvec3(40.0f, 40.0f, 40.0f));
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


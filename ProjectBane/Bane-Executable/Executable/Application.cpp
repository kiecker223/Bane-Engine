#include "Application.h"
#include "Rendering/RendererInterface.h"
#include "Rendering/DefferedRenderer.h"
#include "Rendering/BasicForwardRenderer.h"
#include <BaneObject/Scene/SceneManager.h>
#include "Graphics/IO/TextureCache.h"
#include "Graphics/IO/ShaderCache.h"
#include <Platform/System/Logging/Logger.h>
#include <Platform/System/Process.h>
#include <Code/EntryPoint.h>
#include <iostream>
#include <sys/timeb.h>
#include <sys/utime.h>
#include <sstream>

#pragma warning(disable:4049)

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

	if (!m_Window)
	{
		uint32 ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
		uint32 ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		ScreenWidth = uint32(ScreenWidth *  (float)0.9);
		ScreenHeight = uint32(ScreenHeight * (float)0.9);
		OpenApplicationWindow("Unnamed window", ScreenWidth, ScreenHeight);
	}

	ApiRuntime::CreateRuntime();
	GetApiRuntime()->Initialize(m_Window);

	char str[256];
	GetCurrentDirectoryA(256, str);
	UNUSED(str);

 	IProcessHandle* Handle = StartProcess("BaneShaderCompiler.exe", { "Dx12", "Shaders/", "CompiledShaders/" });
 	Handle->WaitForFinish();
 	
	InitShaderCache("CompiledShaders/ShaderPipelines.json");
	InitializeTextureCache();

	InitSceneManager();
	
	if (m_SceneRenderer == nullptr)
	{
		m_SceneRenderer = new BasicForwardRenderer();
	}
	m_SceneRenderer->Initialize(m_Window);
	
	m_StartCallback = &InitApplication;
	m_StartCallback();
}

void Application::Run()
{	
	using Clock = std::chrono::high_resolution_clock;
	auto TimeStart = Clock::now();

	while (!m_Window->QuitRequested())
	{
		Scene* pCurrentScene = GetSceneManager()->CurrentScene;
		
		RenderLoop RL;
		RL.SetSkybox({ pCurrentScene->GetSkybox(), float3(0.f, 0.f, 0.f) });
		pCurrentScene->Render(RL);
		RL.Draw(); 
		m_SceneRenderer->Submit(RL);
		m_SceneRenderer->Render();

		auto Now = Clock::now();
		auto DeltaTime = Now - TimeStart;
		TimeStart = Now;
		float Dt = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(DeltaTime).count()) / 1E+9f;
		pCurrentScene->Tick(Dt);
		m_SceneRenderer->Present();
		std::cout << "Delta Time: " << Dt << std::endl;
	}
}

void Application::Shutdown()
{
	ShutdownSceneManager();
	DestroyShaderCache();
	DestroyTextureCache();
	m_SceneRenderer->Shutdown();
	delete m_SceneRenderer;
	GlobalLog::Cleanup();
}


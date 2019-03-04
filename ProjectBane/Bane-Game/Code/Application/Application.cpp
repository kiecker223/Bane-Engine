#include "Application.h"
#include "Rendering/RendererInterface.h"
#include "Rendering/BasicForwardRenderer.h"
#include "GraphicsCore/IO/TextureCache.h"
#include "GraphicsCore/IO/ShaderCache.h"
#include <Platform/System/Logging/Logger.h>
#include <Core/Data/Timer.h>
#include <Platform/System/Process.h>
#include <Platform/Input/InputSystem.h>
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
	while (!m_Window->QuitRequested())
	{
		m_SceneRenderer->Render();
		m_SceneRenderer->Present();
		FrameTime.StartTimer();
	}
}

void Application::Shutdown()
{
	DestroyShaderCache();
	DestroyTextureCache();
	m_SceneRenderer->Shutdown();
	delete m_SceneRenderer;
	GlobalLog::Cleanup();
}


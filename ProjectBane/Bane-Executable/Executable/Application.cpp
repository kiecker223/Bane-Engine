#include "Application.h"
#include "Rendering/RendererInterface.h"
#include "Rendering/DefferedRenderer.h"
#include <BaneObject/Scene/SceneManager.h>
#include "Graphics/IO/TextureCache.h"
#include "Graphics/IO/ShaderCache.h"
#include <Platform/System/Logging/Logger.h>
#include <Platform/System/Process.h>
#include <Code/EntryPoint.h>
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
		m_SceneRenderer = new DefferedRenderer();
	}
	m_SceneRenderer->Initialize(m_Window);
	
	m_StartCallback = &InitApplication;
	m_StartCallback();
}

void Application::Run()
{
	while (!m_Window->QuitRequested())
	{
		//m_UpdateCallback();
		GetSceneManager()->CurrentScene->Tick(0.f);
		m_SceneRenderer->Render();
		m_SceneRenderer->Present();
	}
}

void Application::Shutdown()
{
	m_SceneRenderer->Shutdown();
	delete m_SceneRenderer;
	ShutdownSceneManager();
	DestroyShaderCache();
	DestroyTextureCache();
	GlobalLog::Cleanup();
}


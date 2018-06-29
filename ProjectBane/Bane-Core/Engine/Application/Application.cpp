#include "Application.h"
#include "Graphics/Rendering/RendererInterface.h"
#include "Graphics/Rendering/DefferedRenderer.h"
#include "BaneObject/Entity/SceneManager.h"
#include "Graphics/IO/TextureCache.h"
#include "Graphics/IO/ShaderCache.h"
#include <sstream>
#include "Window.h"


Application* Application::GApplication = nullptr;


Application::Application() :
	m_SceneRenderer(nullptr)
{
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

void Application::OpenApplicationWindow(const std::string& Name, uint Width, uint Height)
{
	if (m_Window)
	{
		delete m_Window;
	}
	m_Window = new Window(Name.c_str(), Width, Height);
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
		uint ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
		uint ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		ScreenWidth = uint(ScreenWidth *  (float)0.9);
		ScreenHeight = uint(ScreenHeight * (float)0.9);
		OpenApplicationWindow("Unnamed window", ScreenWidth, ScreenHeight);
	}

	ApiRuntime::CreateRuntime();
	GetApiRuntime()->Initialize();

	InitShaderCache();
	InitializeTextureCache();

	InitSceneManager();

	if (m_SceneRenderer == nullptr)
	{
		m_SceneRenderer = new DefferedRenderer();
	}
	m_SceneRenderer->Initialize();

	m_StartCallback();
}

void Application::Run()
{
	while (!m_Window->QuitRequested())
	{
		m_UpdateCallback();
		GetCurrentScene()->Tick(0.f);
		m_SceneRenderer->Render();
		m_SceneRenderer->Present();
	}
}

void Application::Shutdown()
{
	m_CleanupCallback();
	m_SceneRenderer->Shutdown();
	delete m_SceneRenderer;
	ShutdownSceneManager();
	DestroyShaderCache();
	DestroyTextureCache();
	GlobalLog::Cleanup();
}


#pragma once
#include "Common.h"
#include <string>
#include "System/Window.h"
#include "System/AssemblyLoader.h"


typedef enum EMESH_QUALITY {
	MESH_QUALITY_LOW,
	MESH_QUALITY_MEDIUM,
	MESH_QUALITY_HIGH
} EMESH_QUALITY;

typedef enum ETEXTURE_QUALITY {
	TEXTURE_QUALITY_LOW,
	TEXTURE_QUALITY_MEDIUM,
	TEXTURE_QUALITY_HIGH
} ETEXTURE_QUALITY;

typedef enum ERENDERING_API {
	RENDERING_API_DIRECT3D12,
	RENDERING_API_VULKAN
} ERENDERING_API;

typedef struct RENDERING_SETTINGS {
	bool				bEnableFXAA;
	bool				bEnableTAA;
	bool				bEnableBloom;
	bool				bEnableAO;
	bool				bEnableShadows;
	ETEXTURE_QUALITY	TextureQuality;
	EMESH_QUALITY		MeshQuality;
	uint32				MaxLights;
} RENDERING_SETTINGS;


class ISceneRenderer;

typedef void(*PFNApplicationStartCallback)();
typedef void(*PFNApplicationUpdateCallback)();
typedef void(*PFNApplicationCleanupCallback)();


class Application
{
public:
	Application();
	Application(int argc, char** argv);
	~Application();

	static bool RegisterApplication(Application* App);
	inline static Application* GetInstance()
	{
		return GApplication;
	}

	inline ISceneRenderer* GetSceneRenderer()
	{
		return m_SceneRenderer;
	}

	inline Window* GetWindow()
	{
		return m_Window;
	}

	inline void SetStartCallback(PFNApplicationStartCallback StartCallback)
	{
		m_StartCallback = StartCallback;
	}

	inline bool HasCustomUpdateFunction() const
	{
		return m_UpdateCallback != nullptr;
	}

	inline PFNApplicationUpdateCallback GetUpdateCallback()
	{
		return m_UpdateCallback;
	}

	inline void SetUpdateCallback(PFNApplicationUpdateCallback UpdateCallback)
	{
		m_UpdateCallback = UpdateCallback;
	}

	inline void SetCleanupCallback(PFNApplicationCleanupCallback CleanupCallback)
	{
		m_CleanupCallback = CleanupCallback;
	}

	void OpenApplicationWindow(const std::string& Name, uint32 Width, uint32 Height);

	void InitSystems();

	void Run();

	void Shutdown();
	
	inline void SetSceneRenderer(ISceneRenderer* InSceneRenderer)
	{
		m_SceneRenderer = InSceneRenderer;
	}

private:
	
	static Application* GApplication;

	PFNApplicationStartCallback m_StartCallback = (PFNApplicationStartCallback)0;
	PFNApplicationUpdateCallback m_UpdateCallback = (PFNApplicationUpdateCallback)0;
	PFNApplicationCleanupCallback m_CleanupCallback = (PFNApplicationCleanupCallback)0;

	Assembly* m_GameAssembly;
	ISceneRenderer* m_SceneRenderer;
	Window* m_Window = nullptr;

};

inline Application* GetApplicationInstance()
{
	return Application::GetInstance();
}

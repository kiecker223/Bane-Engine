#pragma once

#include "Core/Common.h"
#include "Scene.h"


class SceneManager
{
	friend SceneManager* GetSceneManager();
	friend void InitSceneManager();
	friend void ShutdownSceneManager();

public:

	// For now just load a new scene?
	Scene* LoadScene(const std::string& SceneName);

private:

	static SceneManager* GInstance;
};

ForceInline void InitSceneManager()
{
	SceneManager::GInstance = new SceneManager();
}

ForceInline void ShutdownSceneManager()
{
	delete SceneManager::GInstance;
}

ForceInline SceneManager* GetSceneManager()
{
	return SceneManager::GInstance;
}
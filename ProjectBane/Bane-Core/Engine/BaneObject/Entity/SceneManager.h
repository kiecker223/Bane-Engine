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
	Scene* CreateNewScene(const std::string& SceneName);

private:

	static SceneManager* GInstance;
};

inline void InitSceneManager()
{
	SceneManager::GInstance = new SceneManager();
}

inline void ShutdownSceneManager()
{
	delete SceneManager::GInstance;
}

inline SceneManager* GetSceneManager()
{
	return SceneManager::GInstance;
}
#pragma once

#include "Common.h"
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
	Scene* CurrentScene;

private:
	static SceneManager* GSceneManagerInstance;

};

inline void InitSceneManager()
{
	SceneManager::GSceneManagerInstance = new SceneManager();
}

inline void ShutdownSceneManager()
{
	delete SceneManager::GSceneManagerInstance;
}

inline SceneManager* GetSceneManager()
{
	return SceneManager::GSceneManagerInstance;
}

#include "SceneManager.h"

SceneManager* SceneManager::GInstance = nullptr;


Scene* SceneManager::LoadScene(const std::string& SceneName)
{
	UNUSED(SceneName);
//	Scene* Result = new Scene();
//	SetCurrentScene(Result);
//	return Result;
	return nullptr;
}

Scene* SceneManager::CreateNewScene(const std::string& SceneName)
{
	UNUSED(SceneName);
	Scene* Result = new Scene();
	if (CurrentScene)
	{
		CurrentScene->DumpScene();
		delete CurrentScene;
	}
	CurrentScene = Result;
	CurrentScene->InitScene();
	return Result;
}


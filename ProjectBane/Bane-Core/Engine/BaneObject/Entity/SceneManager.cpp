#include "SceneManager.h"


SceneManager* SceneManager::GInstance = nullptr;

Scene* SceneManager::LoadScene(const std::string& SceneName)
{
	Scene* Result = new Scene();
	SetCurrentScene(Result);
	return Result;
}

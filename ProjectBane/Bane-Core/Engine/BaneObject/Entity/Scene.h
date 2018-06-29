#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include "Entity.h"

typedef struct SCENE_DATA {
	void* unused;
} SCENE_DATA;

class Scene
{
	friend Scene* GetCurrentScene();
	friend void SetCurrentScene(Scene* InScene);
public:

	Scene();

	Entity* CreateEntity(const std::string& EntityName);
	Entity* FindEntity(const std::string& EntityName);
	Entity* FindEntity(const EntityIdentifier& EntityId);
	Entity* FindEntity(uint64 Id);

	bool EntityExists(const std::string& EntityName);
	bool EntityExists(const EntityIdentifier& Id);
	bool EntityExists(uint64 Id);

	void Tick(float DT);
	void DumpScene();
	void LoadFromMetaData(const SCENE_DATA* Data);
	void InitScene();
	
	ForceInline Entity* GetSceneRoot()
	{
		return m_Root;
	}

private:

	static Scene* GCurrentScene;

	Entity* m_Root;
	std::map<uint64, Entity*> m_Entities;

};

ForceInline Scene* GetCurrentScene()
{
	return Scene::GCurrentScene;
}

ForceInline void SetCurrentScene(Scene* InScene)
{
	Scene* pScene = GetCurrentScene();
	if (pScene)
	{
		pScene->DumpScene();
		delete pScene;
	}
	Scene::GCurrentScene = InScene;
	pScene = InScene;
	if (InScene)
	{
		// Should setting it to nullptr be valid?
	}
	// For now its not
	BANE_CHECK(pScene);
	pScene->InitScene();
}


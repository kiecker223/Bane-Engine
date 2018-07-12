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
	Scene(const std::string& SceneName);

	inline const std::string& GetName() const
	{
		return m_Name;
	}

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
	
	inline Entity* GetSceneRoot()
	{
		return m_Root;
	}

private:

	static Scene* GCurrentScene;
	std::string m_Name;
	Entity* m_Root;
	struct EntityHashEntry
	{
		uint64 Hash;
		Entity* pEntity;
	};
	std::vector<EntityHashEntry> m_Entities;
};

inline Scene* GetCurrentScene()
{
	return Scene::GCurrentScene;
}

inline void SetCurrentScene(Scene* InScene)
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
		pScene->InitScene();
	}
	// BANE_CHECK(pScene);
}


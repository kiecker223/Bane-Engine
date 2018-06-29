#include "Scene.h"

Scene* Scene::GCurrentScene = nullptr;


static std::string GenerateNewName(std::string OriginalName, uint Count)
{
	return std::string(OriginalName + " (" + std::to_string(Count) + ")");
}

Scene::Scene()
{
	m_Root = new Entity(EntityIdentifier("SceneRoot"));
}

Entity* Scene::CreateEntity(const std::string& EntityName)
{
	std::string SearchName = EntityName;
	Entity* pEntity = FindEntity(SearchName);
	// Slow
	uint Index = 1;
	while (pEntity)
	{
		SearchName = GenerateNewName(SearchName, Index);
		pEntity = FindEntity(SearchName);
		Index++;
	}
	pEntity = new Entity(EntityIdentifier(SearchName));
	auto p = std::pair<uint64, Entity*>(pEntity->GetId().HashedName, pEntity);
	m_Entities.insert(p);
	return pEntity;
}

Entity* Scene::FindEntity(const std::string& EntityName)
{
	return FindEntity(EntityIdentifier(EntityName));
}

Entity* Scene::FindEntity(const EntityIdentifier& EntityId)
{
	return FindEntity(EntityId.HashedName);
}

Entity* Scene::FindEntity(uint64 Id)
{
	auto FoundRes = m_Entities.find(Id);
	if (FoundRes == m_Entities.end())
	{
		return nullptr;
	}
	return FoundRes->second;
}

bool Scene::EntityExists(const std::string& EntityName)
{
	return EntityExists(EntityIdentifier(EntityName));
}

bool Scene::EntityExists(const EntityIdentifier& Id)
{
	return EntityExists(Id.HashedName);
}

bool Scene::EntityExists(uint64 Id)
{
	auto FoundRes = m_Entities.find(Id);
	if (FoundRes == m_Entities.end())
	{
		return nullptr;
	}
	return FoundRes->second;
}

void Scene::Tick(float DT)
{
	for (auto& e : m_Entities)
	{
		e.second->Tick(DT);
	}
}

void Scene::DumpScene()
{
	for (auto& e : m_Entities)
	{
		delete e.second;
	}
	m_Entities.clear();
}

void Scene::LoadFromMetaData(const SCENE_DATA* Data)
{

}

void Scene::InitScene()
{
	for (auto& e : m_Entities)
	{
		e.second->SubmitRenderingComponents();
	}
}


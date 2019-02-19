#include "Scene.h"
#include "Graphics/Data/RenderLoop.h"
#include <algorithm>
#include <iostream>


static std::string GenerateNewName(std::string OriginalName, uint32 Count)
{
	return std::string(OriginalName + " (" + std::to_string(Count) + ")");
}

Scene::Scene()
{
	m_Root = new Entity(EntityIdentifier("SceneRoot"));
	m_Entities.push_back({ m_Root->GetId().HashedName, m_Root });
}

Scene::Scene(const std::string& SceneName) :
	m_Name(SceneName)
{
	m_Root = new Entity(EntityIdentifier("SceneRoot"));
	m_Entities.push_back({ m_Root->GetId().HashedName, m_Root });
}

Entity* Scene::CreateEntity(const std::string& EntityName)
{
	Entity* pEntity = FindEntity(EntityName);
	if (pEntity == nullptr)
	{
		auto Iter = std::find_if(m_EntityAddList.begin(), m_EntityAddList.end(), [EntityName](const EntityHashEntry& EntityToFind)
		{
			if (EntityToFind.pEntity->GetId().Name == EntityName)
			{
				return true;
			}
			return false;
		});
		if (Iter != m_EntityAddList.end())
		{
			pEntity = Iter->pEntity;
		}
	}
	std::string FinalName;
	// Slow
	uint32 Index = 1;
	if (pEntity)
	{
		while (pEntity)
		{
			std::string SearchName = GenerateNewName(EntityName, Index);
			pEntity = FindEntity(SearchName);
			Index++;
			if (!pEntity)
			{
				FinalName = SearchName;
			}
		}
	}
	else
	{
		FinalName = EntityName;
	}
	pEntity = new Entity(EntityIdentifier(FinalName));
	pEntity->m_SceneOwner = this;
	pEntity->SetParent(m_Root);
	EntityHashEntry Entry = { pEntity->GetId().HashedName, pEntity };
	m_EntityAddList.push_back(Entry);
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
	auto FoundRes = m_EntitySearchList.find(Id);
	if (FoundRes == m_EntitySearchList.end())
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
	return std::find_if(m_Entities.begin(), m_Entities.end(), 
		[&](const EntityHashEntry& Entry) 
		{
			if (Entry.Hash == Id)
			{
				return true;
			}
			return false;
		}
	) != m_Entities.end();
}

void Scene::Tick(double DT)
{
	m_Root->Tick(DT);
	if (m_EntityAddList.size() > 0)
	{
		for (auto& e : m_EntityAddList)
		{
			m_Entities.push_back(e);
			m_EntityStartList.push_back(e);
			if (m_EntitySearchList.find(e.Hash) == m_EntitySearchList.end())
			{
				m_EntitySearchList[e.Hash] = e.pEntity;
			}
			else
			{
				__debugbreak();
			}
		}
		m_EntityAddList.clear();
	}
	if (m_EntityStartList.size() > 0)
	{
		for (uint32 i = 0; i < m_EntityStartList.size(); i++)
		{
			m_EntityStartList[i].pEntity->Start();
		}
		m_EntityStartList.clear();
	}
	if (m_World.IsReadyForRead())
	{
		m_Root->PhysicsTick();
//		std::lock_guard<std::mutex> ScopedLock(m_World.GenerateOctTreeMutex);
//		m_PhysOctree = m_World.GetOctTree();
	}
}


// void DrawOctsImpl(PhysicsWorld::OctTreeType::TNode* pNewNode, RenderLoop& RL, uint32& CallDepth)
// {
// 	using TNode = PhysicsWorld::OctTreeType::TNode;
// 	CallDepth++;
// 	for (uint32 i = 0; i < 8; i++)
// 	{
// 		if (pNewNode->Children[i])
// 		{
// 			DrawOctsImpl(pNewNode->Children[i], RL, CallDepth);
// 		}
// 	}
// 	RL.AddBoundingBox(pNewNode->Value.Bounds);
// }

void Scene::Render(RenderLoop& RL)
{
	for (auto& e : m_Entities)
	{
		e.pEntity->UpdateRenderObjects(RL);
	}
	if (bDrawPhysicsDebugInfo)
	{
//		uint32 CallDepth;
//		DrawOctsImpl(m_PhysOctree.Base, RL, CallDepth);
//		UNUSED(CallDepth);
	}
}

void Scene::DumpScene()
{
	m_World.bRunningPhysicsSim = false;
	m_MeshCache.Destroy();
	for (auto& e : m_Entities)
	{
		delete e.pEntity;
	}
	m_Entities.clear();
	m_World.DestroyThread();
}

void Scene::LoadFromMetaData(const SCENE_DATA* Data)
{
	UNUSED(Data);
}

void Scene::InitScene()
{
	m_MeshCache.Initialize();
	for (auto& e : m_Entities)
	{
		e.pEntity->SubmitRenderingComponents();
	}
	m_World.SpawnThread();
}

void Scene::DeleteEntity(Entity** ppEntity)
{
	UNUSED(ppEntity);
}

bool Scene::Raycast(const double3& RayStart, const double3& Direction, double MaxDistance, RayHitInformation& HitInfo)
{
	RAY_HIT_INFO RayHitInfo;
	if (m_World.CastRay({ RayStart, Direction, MaxDistance }, RayHitInfo))
	{
		HitInfo.Normal = RayHitInfo.Normal;
		HitInfo.Position = RayHitInfo.Position;
		HitInfo.HitEntity = FindEntity(RayHitInfo.Body.EntityHandle);
		m_NodesIntersected = m_World.DebugRayCastIntersectedNodes;
		return true;
	}
	return false;
}

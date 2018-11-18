#include "Scene.h"
#include "Graphics/Data/RenderLoop.h"
#include <algorithm>


static std::string GenerateNewName(std::string OriginalName, uint32 Count)
{
	return std::string(OriginalName + " (" + std::to_string(Count) + ")");
}

Scene::Scene()
{
	m_Root = new Entity(EntityIdentifier("SceneRoot"));
	m_Entities.Add({ m_Root->GetId().HashedName, m_Root });
}

Scene::Scene(const std::string& SceneName) :
	m_Name(SceneName)
{
	m_Root = new Entity(EntityIdentifier("SceneRoot"));
	m_Entities.Add({ m_Root->GetId().HashedName, m_Root });
}

Entity* Scene::CreateEntity(const std::string& EntityName)
{
	Entity* pEntity = FindEntity(EntityName);
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
	m_EntityAddList.Add(Entry);
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
	auto FoundRes = std::find_if(m_Entities.begin(), m_Entities.end(),
		[&](const EntityHashEntry& Entry)
		{
			if (Entry.Hash == Id)
			{
				return true;
			}
			return false;
		}
	);
	if (FoundRes == m_Entities.end())
	{
		return nullptr;
	}
	return FoundRes->pEntity;
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


void Scene::Tick(float DT)
{
	for (auto& e : m_Entities)
	{
		e.pEntity->Tick(DT);
	}
	if (m_EntityAddList.GetCount() > 0)
	{
		for (auto& e : m_EntityAddList)
		{
			m_Entities.Add(e);
			m_EntityStartList.Add(e);
		}
		m_EntityAddList.Empty();
	}
	if (m_EntityStartList.GetCount() > 0)
	{
		for (uint32 i = 0; i < m_EntityStartList.GetCount(); i++)
		{
			m_EntityStartList[i].pEntity->Start();
		}
		m_EntityStartList.Empty();
	}
}


void DrawOctsImpl(TBinaryTree<PhysicsWorld::OctTreeNode>::TNode* pNewNode, RenderLoop& RL)
{
	BoundingBox NewBox = pNewNode->Value.Bounds;
	RL.AddBoundingBox(NewBox);
	if (pNewNode->NextL)
	{
		DrawOctsImpl(pNewNode->NextL, RL);
	}
	if (pNewNode->NextR)
	{
		DrawOctsImpl(pNewNode->NextR, RL);
	}
}

void Scene::Render(RenderLoop& RL)
{
	for (auto& e : m_Entities)
	{
		e.pEntity->UpdateRenderObjects(RL);
	}
	if (bDrawPhysicsDebugInfo)
	{
		if (m_World.IsReadyForRead())
		{
			std::lock_guard<std::mutex> ScopedLock(m_World.GenerateOctTreeMutex);
			m_PhysOctree = m_World.GetOctTree();
		}
		DrawOctsImpl(m_PhysOctree.Tail, RL);
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
	m_Entities.ClearMemory();
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



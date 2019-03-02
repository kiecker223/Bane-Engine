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
	m_EntitiesAllocated = 0;
	m_Entities.resize(10000);
}

Scene::Scene(const std::string& SceneName) :
	m_Name(SceneName)
{
	m_EntitiesAllocated = 0;
	m_Entities.resize(10000);
}

Entity* Scene::CreateEntity(const std::string& EntityName)
{
	std::string FinalName;
	if (EntityExists(EntityName))
	{
		for (uint32 i = 0; ; i++)
		{
			std::string TestingName = GenerateNewName(EntityName, i);
			if (!EntityExists(TestingName))
			{
				FinalName = TestingName;
				break;
			}
		}
	}
	else
	{
		FinalName = EntityName;
	}
	Entity* Result = &m_Entities[m_EntitiesAllocated];
	m_EntitiesAllocated++;
	if (Result->GetComponentCount())
	{
		Result->DestroyComponents();
	}
	Result->SetId(EntityIdentifier(FinalName));
	Result->m_SceneOwner = this;
	m_EntityStartList.push_back(Result);
	return Result;
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
	return FindEntity(Id) != nullptr;
}

void Scene::Tick(double DT)
{
	if (!m_EntityStartList.empty())
	{
		for (uint32 i = 0; i < m_EntityStartList.size(); i++)
		{
			m_EntityStartList[i]->Start();
		}
		m_EntityStartList.clear();
	}

	for (uint32 i = 0; i < m_EntitiesAllocated; i++)
	{
		m_Entities[i].Tick(DT);
	}
	if (m_World.IsReadyForRead())
	{
		for (uint32 i = 0; i < m_EntitiesAllocated; i++)
		{
			m_Entities[i].PhysicsTick();
		}
	}
}


void Scene::Render(RenderLoop& RL)
{
	for (uint32 i = 0; i < m_EntitiesAllocated; i++)
	{
		m_Entities[i].UpdateRenderObjects(RL);
	}
	if (bDrawPhysicsDebugInfo)
	{
	}
}

void Scene::DumpScene()
{
	m_World.bRunningPhysicsSim = false;
	m_MeshCache.Destroy();
	for (auto& e : m_Entities)
	{
		e.~Entity();
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
		e.SubmitRenderingComponents();
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

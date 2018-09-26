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
	m_Root->GetPhysicsProperties().bCanTick = false;
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
	std::string SearchName = EntityName;
	Entity* pEntity = FindEntity(SearchName);
	// Slow
	uint32 Index = 1;
	while (pEntity)
	{
		SearchName = GenerateNewName(SearchName, Index);
		pEntity = FindEntity(SearchName);
		Index++;
	}
	pEntity = new Entity(EntityIdentifier(SearchName));
	pEntity->SetParent(m_Root->GetId());
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

float Timer;

void Scene::Tick(float DT)
{
	Timer += DT;
	for (auto& e : m_Entities)
	{
		e.pEntity->Tick(DT);
	}
	if (m_EntityAddList.size() > 0)
	{
		for (auto& e : m_EntityAddList)
		{
			m_Entities.push_back(e);
			e.pEntity->Start();
			auto PhysicsProps = e.pEntity->GetPhysicsProperties();
			if (PhysicsProps.bCanTick)
			{
				PhysicsBody Body;
				Body.Mass = PhysicsProps.Mass;
				Body.Position = e.pEntity->GetTransform()->GetPosition();
				Body.Radius = 1.f;
				Body.Velocity = PhysicsProps.Velocity;
				e.pEntity->GetPhysicsProperties().PhysicsWorldHandle = m_World.AddBody(Body);
			}
		}
		m_EntityAddList.clear();
	}
	if (Timer > 1.f / 60.f)
	{
		m_World.UpdateBodies();
		PhysicsUpdate(m_World.UpdateBuffer);
		Timer = 0.f;
	}
}

void Scene::PhysicsUpdate(const PhysicsUpdateBuffer UpdateBuffer)
{
	for (auto& e : m_Entities)
	{
		if (e.pEntity->GetPhysicsProperties().bCanTick)
		{
			uint32 PhysicsHandle = e.pEntity->GetPhysicsProperties().PhysicsWorldHandle;
			e.pEntity->GetTransform()->SetPosition(UpdateBuffer.Bodies[PhysicsHandle].Position);
			e.pEntity->GetPhysicsProperties().Velocity = UpdateBuffer.Bodies[PhysicsHandle].Velocity;
		}
	}
}

void Scene::Render(RenderLoop& RL)
{
	for (auto& e : m_Entities)
	{
		e.pEntity->UpdateRenderObjects(RL);
	}
}

void Scene::DumpScene()
{
	for (auto& e : m_Entities)
	{
		delete e.pEntity;
	}
	m_Entities.clear();
	m_MeshCache.Destroy();
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
}



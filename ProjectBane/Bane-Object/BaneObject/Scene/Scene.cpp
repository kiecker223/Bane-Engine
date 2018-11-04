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
	m_Entities.Add({ m_Root->GetId().HashedName, m_Root });
}

Scene::Scene(const std::string& SceneName) :
	m_Name(SceneName)
{
	m_Root = new Entity(EntityIdentifier("SceneRoot"));
	m_Root->GetPhysicsProperties().bCanTick = false;
	m_Entities.Add({ m_Root->GetId().HashedName, m_Root });
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
	pEntity->m_SceneOwner = this;
	pEntity->SetParent(m_Root->GetId());
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
	if (m_EntityAddList.GetElementCount() > 0)
	{
		for (auto& e : m_EntityAddList)
		{
			m_Entities.Add(e);
			m_EntityStartList.Add(e);
			auto PhysicsProps = e.pEntity->GetPhysicsProperties();
			if (PhysicsProps.bCanTick)
			{
				//PHYSICS_BODY_CREATE_INFO Body = { };
				//Orientation		;
				//Position		;
				//Velocity		;
				//AngularVelocity	;
				//Mass			;
				//BodyType		;
				//PhysMesh		;
				//Sphere			;
				//EntityHandle	;
				//
				//e.pEntity->GetPhysicsProperties().PhysicsWorldHandle = m_World.AddBody(Body);
			}
		}
		m_EntityAddList.Empty();
	}
	if (m_EntityStartList.GetElementCount() > 0)
	{
		for (uint32 i = 0; i < m_EntityStartList.GetElementCount(); i++)
		{
			m_EntityStartList[i].pEntity->Start();
		}
		m_EntityStartList.Empty();
	}
	if (m_World.IsReadyForRead())
	{
	//	PhysicsUpdate(m_World.UpdateBuffer);
	}
}

void Scene::PhysicsUpdate(const PhysicsUpdateBuffer& UpdateBuffer)
{
	if (UpdateBuffer.Bodies.GetElementCount() > 0)
	{
		for (auto& e : m_Entities)
		{
			if (e.pEntity->GetPhysicsProperties().bCanTick)
			{
				uint32 PhysicsHandle = e.pEntity->GetPhysicsProperties().PhysicsWorldHandle;
				auto& Body = UpdateBuffer.Bodies[PhysicsHandle];
				e.pEntity->GetTransform()->SetPosition(Body.Position);
				e.pEntity->GetPhysicsProperties().Velocity = UpdateBuffer.Bodies[PhysicsHandle].Velocity;
				auto& CurrentRotation = e.pEntity->GetTransform()->GetRotation();
				double RadsPerSecond = length(Body.AngularVelocity);
				if (!isnan(RadsPerSecond) && abs(RadsPerSecond) > 0.0f)
				{
					CurrentRotation *= Quaternion::FromAxisAngle(normalized(Body.AngularVelocity), RadsPerSecond);
					CurrentRotation.Normalize();
				}
			}
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
	m_World.MessageQueue.AllocMessage<PhysicsMessage>(true);
	m_MeshCache.Destroy();
	for (auto& e : m_Entities)
	{
		delete e.pEntity;
	}
	m_Entities.Empty();
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
	// m_World.SpawnThread();
}



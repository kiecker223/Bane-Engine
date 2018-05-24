#include "EntityManager.h"
#include "Core/Systems/Logging/Logger.h"



EntityManager* EntityManager::GEntityManager = nullptr;

void EntityManager::Initialize()
{
	GEntityManager = new EntityManager();
}

void EntityManager::Shutdown()
{
	delete GEntityManager;
}

Entity* EntityManager::CreateEntity(const char* Name)
{
	Entity* Ent = new Entity(EntityIdentifier(Name));
	m_Entities.emplace(std::pair<EntityIdentifier, Entity*>(Ent->GetId(), Ent));
	Ent->Start();
	return Ent;
}

Entity* EntityManager::FindEntity(const char* Name)
{
	EntityIdentifier Id(Name);
	return FindEntity(Id);
}

Entity* EntityManager::FindEntity(const EntityIdentifier& Name)
{
	Entity* pEntity = nullptr;
	if (!EntityExists(Name, &pEntity, nullptr))
	{
		BaneLog() << "Failed to find entity: " << Name.Name;
	}
	return pEntity;
}

bool EntityManager::EntityExists(const char* Name, Entity** OutEntity, EntityIdentifier* OutID)
{
	return EntityExists(EntityIdentifier(Name), OutEntity, OutID);
}

bool EntityManager::EntityExists(const EntityIdentifier& Name, Entity** OutEntity, EntityIdentifier* OutID)
{
	auto FoundResult = m_Entities.find(Name);
	bool Result = FoundResult != m_Entities.end();
	if (Result)
	{
		if (OutID)
		{
			*OutID = FoundResult->first;
		}
		if (OutEntity)
		{
			*OutEntity = FoundResult->second;
		}
	}
	return Result;
}

void EntityManager::DeleteEntity(Entity* pEntity)
{
	EntityIdentifier id = pEntity->GetId();
	m_Entities.erase(id);
	delete pEntity;
}

void EntityManager::DeleteEntity(const char* Name)
{
	return DeleteEntity(EntityIdentifier(Name));
}

void EntityManager::DeleteEntity(const EntityIdentifier& Name)
{
	Entity* pEntity = nullptr;
	if (EntityExists(Name, &pEntity, nullptr))
	{
		return DeleteEntity(pEntity);
	}
	BaneLog() << "Failed to find entity: " << Name.Name;
}
 
void EntityManager::TickOverEntities(double DT)
{
	for (auto it : m_Entities)
	{
		Entity* Ent = it.second;
		Ent->Tick(DT);
	}
}

void EntityManager::SubmitRenderingFeatures()
{
	for (auto it : m_Entities)
	{
		Entity* Ent = it.second;
		Ent->SubmitRenderingComponents();
		Ent->Start();
	}
}

void EntityManager::DeleteAllEntities()
{
	for (auto it : m_Entities)
	{
		Entity* Ent = it.second;
		DeleteEntity(Ent);
	}
}


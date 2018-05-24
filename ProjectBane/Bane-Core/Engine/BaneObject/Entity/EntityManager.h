#pragma once
#include "Core/Common.h"
#include "Entity.h"
#include <map>

class EntityManager
{
public:

	ForceInline static EntityManager* GetInstance()
	{
		return GEntityManager;
	}

	static void Initialize();
	static void Shutdown();

	Entity* CreateEntity(const char* Name);
	Entity* FindEntity(const char* Name);
	Entity* FindEntity(const EntityIdentifier& Name);

	bool EntityExists(const char* Name, Entity** OutEntity, EntityIdentifier* OutID);
	bool EntityExists(const EntityIdentifier& Name, Entity** OutEntity, EntityIdentifier* OutID);

	void DeleteEntity(Entity* pEntity);
	void DeleteEntity(const char* Name);
	void DeleteEntity(const EntityIdentifier& Name);

	void TickOverEntities(double DT);
	void SubmitRenderingFeatures();

private:

	void DeleteAllEntities();

	std::map<EntityIdentifier, Entity*> m_Entities;
	
	static EntityManager* GEntityManager;
};

ForceInline EntityManager* GetEntityManager()
{
	return EntityManager::GetInstance();
}
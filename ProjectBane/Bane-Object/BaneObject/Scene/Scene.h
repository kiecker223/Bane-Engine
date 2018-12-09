#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include "Graphics/IO/MeshCache.h"
#include "Common/Types.h"
#include "Common/Hash.h"
#include "../Entity/Entity.h"
#include "Physics/PhysicsWorld.h"

typedef struct SCENE_DATA {
	void* unused;
} SCENE_DATA;

class RayHitInformation
{
public:
	double3 Position;
	double3 Normal;
	Entity* HitEntity;
};

class Scene
{
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
	//void PhysicsUpdate(const PhysicsUpdateBuffer& UpdateBuffer);
	void Render(RenderLoop& RL);
	void DumpScene();
	void LoadFromMetaData(const SCENE_DATA* Data);
	void InitScene();
	void DeleteEntity(Entity** pEntity);

	inline void SetCamera(Entity* EntityCamera)
	{
		m_CameraEntity = EntityCamera;
	}

	inline Entity* GetCameraEntity()
	{
		return m_CameraEntity;
	}

	inline void SetSkybox(ITextureCube* InSkybox)
	{
		m_Skybox = InSkybox;
	}

	inline const ITextureCube* GetSkybox() const
	{
		return m_Skybox;
	}

	inline MeshCache& GetMeshCache()
	{
		return m_MeshCache;
	}
	
	inline Entity* GetSceneRoot()
	{
		return m_Root;
	}

	bool Raycast(const double3& RayStart, const double3& Direction, double MaxDistance, RayHitInformation& HitInfo);

	inline PhysicsWorld& GetPhysicsWorld()
	{
		return m_World;
	}

	bool bDrawPhysicsDebugInfo = false;

private:

	MeshCache m_MeshCache;
	PhysicsWorld m_World;

	// TODO: Remove me when you're done debugging physics stuff.
	// please!!
	PhysicsWorld::OctTreeType m_PhysOctree;
	ITextureCube* m_Skybox;
	std::string m_Name;
	Entity* m_Root;
	Entity* m_CameraEntity;
	struct EntityHashEntry
	{
		uint64 Hash;
		Entity* pEntity;
	};
	std::unordered_map<uint64, Entity*> m_EntitySearchList;
	std::vector<EntityHashEntry> m_Entities;
	std::vector<EntityHashEntry> m_EntityAddList;
	std::vector<EntityHashEntry> m_EntityStartList;
};

// Scene* GetCurrentScene();
// void SetCurrentScene(Scene* InScene);

#pragma once
#include <Core/Containers/Array.h>
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
	void PhysicsUpdate(const PhysicsUpdateBuffer& UpdateBuffer);
	void Render(RenderLoop& RL);
	void DumpScene();
	void LoadFromMetaData(const SCENE_DATA* Data);
	void InitScene();

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
	TBinaryTree<PhysicsWorld::OctTreeNode> m_PhysOctree;
	ITextureCube* m_Skybox;
	std::string m_Name;
	Entity* m_Root;
	Entity* m_CameraEntity;
	struct EntityHashEntry
	{
		uint64 Hash;
		Entity* pEntity;
	};
	TArray<EntityHashEntry> m_Entities;
	TArray<EntityHashEntry> m_EntityAddList;
	TArray<EntityHashEntry> m_EntityStartList;
};

// Scene* GetCurrentScene();
// void SetCurrentScene(Scene* InScene);

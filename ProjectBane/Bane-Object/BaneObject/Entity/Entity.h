#pragma once
#include "Transform.h"
#include "Common.h"
#include "../Components/ComponentBase.h"
#include "../Components/ComponentAllocator.h"
#include "Graphics/Data/RenderLoop.h"
#include "Common/Hash.h"
#include <array>
#include "PhysicsProperties.h"

#pragma warning(disable:4049)

#define ENTITY_MAX_CHILDREN 20

class EntityIdentifier
{
public:
	EntityIdentifier() :
		HashedName(0)
	{
	}
	
	EntityIdentifier(const std::string& InName)
	{
		HashedName = GetDJB264BitHash(InName.c_str(), InName.length());
		Name = InName;
	}

	EntityIdentifier(uint64 InID) :
		HashedName(InID)
	{
		__debugbreak();
	}

	EntityIdentifier(const EntityIdentifier& ID) :
		HashedName(ID.HashedName),
		Name(ID.Name)
	{
	}

	inline EntityIdentifier& operator = (const EntityIdentifier& Other)
	{
		HashedName = Other.HashedName;
		Name = Other.Name;
		return *this;
	}

	inline operator uint64& ()
	{
		return HashedName;
	}

	inline operator uint64& () const
	{
		return HashedName;
	}

	inline bool IsValid() const
	{
		return HashedName != 0;
	}

	inline operator bool() const
	{
		return IsValid();
	}

	mutable uint64 HashedName;
	std::string Name;
};

inline bool operator == (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return Left.HashedName == Right.HashedName;
}

inline bool operator != (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return !(Left == Right);
}

inline bool operator > (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return (Left.HashedName > Right.HashedName);
}

inline bool operator < (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return (Left.HashedName < Right.HashedName);
}

inline bool operator >= (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return (Left.HashedName >= Right.HashedName);
}

inline bool operator <= (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return (Left.HashedName <= Right.HashedName);
}

template<typename T>
class TComponentHandle
{
	template<typename T>
	friend class TComponentHandle;
public:

	inline TComponentHandle() : m_AllocObjects(nullptr), m_Index(0) { }

	template<typename TOther>
	inline TComponentHandle(const TComponentHandle<TOther>& Other)
	{
		m_AllocObjects = Other.m_AllocObjects;
		m_Index = Other.m_Index;
	}

	inline TComponentHandle(const TComponentHandle& Other)
	{
		m_AllocObjects = Other.m_AllocObjects;
		m_Index = Other.m_Index;
	}

 	inline TComponentHandle(TComponentHandle&& Other)
 	{
 		m_AllocObjects = Other.m_AllocObjects;
 		m_Index = Other.m_Index;
 	}

	inline TComponentHandle(const nullptr_t Other) : m_AllocObjects(nullptr), m_Index(0)
	{
		UNUSED(Other);
	}

	inline TComponentHandle(std::vector<Component*>* AllocObjects, uint32 Index)
	{
		m_AllocObjects = AllocObjects;
		m_Index = Index;
	}

	template<typename TOther>
	inline TComponentHandle<T>& operator = (const TComponentHandle<TOther>& Other)
	{
		m_AllocObjects = Other.m_AllocObjects;
		m_Index = Other.m_Index;
		return *this;
	}
	
	inline TComponentHandle& operator = (const TComponentHandle& Other)
	{
		m_AllocObjects = Other.m_AllocObjects;
		m_Index = Other.m_Index;
		return *this;
	}

	inline TComponentHandle<T>& operator = (const nullptr_t Other) 
	{
		UNUSED(Other);
		m_AllocObjects = nullptr;
		m_Index = 0;
		return *this;
	}

	inline T* operator ->()
	{
#ifdef _DEBUG
		BANE_CHECK(m_AllocObjects);
#endif
		return static_cast<T*>((*m_AllocObjects)[m_Index]);
	}

	inline T& operator ->() const
	{
#ifdef _DEBUG
		BANE_CHECK(m_AllocObjects);
#endif
		return static_cast<T*>((*m_AllocObjects)[m_Index]);
	}

private:

	uint32 m_Index;
	std::vector<Component*>* m_AllocObjects;

};

class Entity
{
	typedef std::vector<EntityIdentifier> IdentifierList;
	friend class Scene;
	template<typename T>
	friend class TComponentHandle;
public:

	Entity() :
		m_Allocator(2048) // Allocate with 1024 bytes
	{
	}

	Entity(EntityIdentifier Id) :
		m_Allocator(2048),
		m_Id(Id)
	{
	}

	template<typename T>
	inline TComponentHandle<T> GetComponent()
	{
		return GetComponentByHash(T::ClassHash);
	}

	TComponentHandle<Component> GetComponentByHash(uint64 Hash);

	template<typename T>
	inline TComponentHandle<T> AddComponent()
	{
		T* RetPointer = m_Allocator.AllocateObject<T>();
		TComponentHandle<T> Result(&m_Allocator.GetAllocatedObjects(), static_cast<uint32>(m_Components.size()));
		m_Components.push_back(T::ClassHash);
		RetPointer->m_Owner = this;
		RetPointer->m_Transform = &m_Transform;
		RetPointer->m_Scene = m_SceneOwner;
		RetPointer->Awake();
		return Result;
	}

	void SubmitRenderingComponents();
	void UpdateRenderObjects(RenderLoop& RL);

	template<class T, class... U>
	inline TComponentHandle<T> AddAndConstructComponent(U&&... Params)
	{
		T* RetPointer = m_Allocator.AllocateObjectCtor<T>(Params...);
		TComponentHandle<T> Result(&m_Allocator.GetAllocatedObjects(), m_Components.size());
		m_Components.push_back(T::ClassHash);
		RetPointer->m_Owner = this;
		RetPointer->m_Transform = &m_Transform;
		RetPointer->m_Scene = m_SceneOwner;
		RetPointer->Awake();
		return Result;
	}

	void RemoveComponent(uint64 ComponentHash);

	template<typename T>
	inline void RemoveComponentByType()
	{
		RemoveComponent(T::ClassHash);
	}

	void AddChild(EntityIdentifier Child);
	void RemoveChild(uint32 ChildIndex);
	float4x4 GetMatrixAffectedByParents() const;

	IdentifierList& GetChildren()
	{
		return m_Children;
	}

	EntityIdentifier GetId() const
	{
		return m_Id;
	}

	inline void Start()
	{
		for (uint32 i = 0; i < m_Allocator.GetAllocatedObjects().size(); i++)
			m_Allocator.GetAllocatedObjects()[i]->Start();
	}

	inline void Tick(float DT)
	{
		for (uint32 i = 0; i < m_Allocator.GetAllocatedObjects().size(); i++)
			m_Allocator.GetAllocatedObjects()[i]->Tick(DT);
	}

	inline Transform* GetTransform() const
	{
		return (Transform*)&m_Transform;
	}

	inline void SetParent(EntityIdentifier Parent)
	{
		m_Parent = Parent;
	}

	Entity* GetChild(uint32 Idx);

	inline uint32 GetChildCount()
	{
		return static_cast<uint32>(m_Children.size());
	}

	inline PhysicsProperties& GetPhysicsProperties()
	{
		return m_PhysicsProperties;
	}

private:

	friend class Entity;

	Entity* GetParent() const;
	
	PhysicsProperties m_PhysicsProperties;
	Transform m_Transform;
	EntityIdentifier m_Id;
	EntityIdentifier m_Parent;
	IdentifierList m_Children;
	Scene* m_SceneOwner;
	ComponentAllocator m_Allocator;
	std::vector<uint64> m_Components;
};

Entity* GetEntityById(const EntityIdentifier& Id);
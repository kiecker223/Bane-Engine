#pragma once
#include "Transform.h"
#include "Common.h"
#include "../Components/ComponentBase.h"
#include "../Components/ComponentAllocator.h"
#include "Graphics/Data/RenderLoop.h"
#include "Common/Hash.h"
#include <array>
#include <string>
#include <Physics/PhysicsProperties.h>

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

	inline TComponentHandle() : m_AllocObjectsStart(nullptr), m_Offset(0) { }

	template<typename TOther>
	inline TComponentHandle(const TComponentHandle<TOther>& Other)
	{
		m_Offset = Other.m_Offset;
		m_AllocObjectsStart = Other.m_AllocObjectsStart;
	}

	inline TComponentHandle(const TComponentHandle& Other)
	{
		m_Offset = Other.m_Offset;
		m_AllocObjectsStart = Other.m_AllocObjectsStart;
	}

 	inline TComponentHandle(TComponentHandle&& Other)
 	{
 		m_Offset = Other.m_Offset;
		m_AllocObjectsStart = Other.m_AllocObjectsStart;
 	}

	inline TComponentHandle(const nullptr_t Other) : m_AllocObjectsStart(nullptr), m_Offset(0)
	{
		UNUSED(Other);
	}

	inline TComponentHandle(uint8** AllocObjectsStart, ptrdiff_t Offset)
	{
		m_Offset = Offset;
		m_AllocObjectsStart = AllocObjectsStart;
	}

	template<typename TOther>
	inline TComponentHandle<T>& operator = (const TComponentHandle<TOther>& Other)
	{
		m_Offset = Other.m_Offset;
		m_AllocObjectsStart = Other.m_AllocObjectsStart;
		return *this;
	}
	
	inline TComponentHandle& operator = (const TComponentHandle& Other)
	{
		m_Offset = Other.m_Offset;
		m_AllocObjectsStart = Other.m_AllocObjectsStart;
		return *this;
	}

	inline TComponentHandle<T>& operator = (const nullptr_t Other) 
	{
		UNUSED(Other);
		m_Offset = 0;
		m_AllocObjectsStart = nullptr;
		return *this;
	}

	inline operator bool()
	{
		return m_AllocObjectsStart;
	}

	inline T* operator ->()
	{
#ifdef _DEBUG
		BANE_CHECK(m_AllocObjectsStart);
#endif
		return reinterpret_cast<T*>(reinterpret_cast<ptrdiff_t>(*m_AllocObjectsStart) + m_Offset);
	}

	inline T& operator ->() const
	{
#ifdef _DEBUG
		BANE_CHECK(m_AllocObjectsStart);
#endif
		return reinterpret_cast<T*>(reinterpret_cast<ptrdiff_t>(*m_AllocObjectsStart) + m_Offset);
	}

private:

	ptrdiff_t m_Offset;
	uint8** m_AllocObjectsStart;
};

class Entity
{
	friend class Scene;
	template<typename T>
	friend class TComponentHandle;
public:

	Entity() :
		m_Parent(nullptr),
		m_Allocator(2048) // Allocate with this parameter's bytes
	{
	}

	Entity(EntityIdentifier Id) :
		m_Parent(nullptr),
		m_ParentChildIdx(-1),
		m_Allocator(2048),
		m_Id(Id)
	{
	}

	inline Entity(const Entity& Rhs)
	{
		m_Transform = Rhs.m_Transform;
		m_Id = Rhs.m_Id;
		m_ParentChildIdx = Rhs.m_ParentChildIdx;
		m_Parent = Rhs.m_Parent;
		m_Children = Rhs.m_Children;
		m_SceneOwner = Rhs.m_SceneOwner;
		m_Allocator = Rhs.m_Allocator;
		m_ComponentStartList = Rhs.m_ComponentStartList;
		m_Components = Rhs.m_Components;
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
		TComponentHandle<T> Result(&m_Allocator.PtrBegin, reinterpret_cast<ptrdiff_t>(m_Allocator.AllocatedObjects[m_Components.size()]));
		m_Components.push_back(T::ClassHash);
		RetPointer->m_Owner = this;
		RetPointer->m_Transform = &m_Transform;
		RetPointer->m_Scene = m_SceneOwner;
		RetPointer->Awake();
		m_ComponentStartList.push_back(Result);
		return Result;
	}

	void SubmitRenderingComponents();
	void UpdateRenderObjects(RenderLoop& RL);

	template<class T, class... U>
	inline TComponentHandle<T> AddAndConstructComponent(U&&... Params)
	{
		T* RetPointer = m_Allocator.AllocateObjectCtor<T>(Params...);
		TComponentHandle<T> Result(&m_Allocator.PtrBegin, reinterpret_cast<ptrdiff_t>(m_Allocator.AllocatedObjects[m_Components.size()]));
		m_Components.push_back(T::ClassHash);
		RetPointer->m_Owner = this;
		RetPointer->m_Transform = &m_Transform;
		RetPointer->m_Scene = m_SceneOwner;
		RetPointer->Awake();
		m_ComponentStartList.push_back(Result);
		return Result;
	}

	void RemoveComponent(uint64 ComponentHash);

	template<typename T>
	inline void RemoveComponentByType()
	{
		RemoveComponent(T::ClassHash);
	}

	void AddChild(Entity* Child);
	void RemoveChild(uint32 ChildIndex);
	float4x4 GetMatrixAffectedByParents() const;

	EntityIdentifier GetId() const
	{
		return m_Id;
	}

	void SetId(const EntityIdentifier& InId)
	{
		m_Id = InId;
	}

	inline void Start()
	{
		for (uint32 i = 0; i < GetComponentCount(); i++) 
		{
			GetComponentByIndex(i)->Start();
			if (!m_ComponentStartList.empty())
			{
				m_ComponentStartList.clear();
			}
		}
	}

	inline void Tick(double DT)
	{
		if (!m_ComponentStartList.empty())
		{
			for (uint32 i = 0; i < m_ComponentStartList.size(); i++)
			{
				m_ComponentStartList[i]->Start();
			}
			m_ComponentStartList.clear();
		}
		for (uint32 i = 0; i < GetComponentCount(); i++) 
		{
			GetComponentByIndex(i)->Tick(DT);
		}
	}

	inline void PhysicsTick()
	{
		for (uint32 i = 0; i < GetComponentCount(); i++) 
		{
			GetComponentByIndex(i)->PhysicsTick();
		}
	}

	inline Transform* GetTransform() const
	{
		return (Transform*)&m_Transform;
	}

	void SetParent(Entity* Parent);

	inline std::vector<Entity*>& GetChildren()
	{
		return m_Children;
	}

	inline Entity* GetChild(uint32 Idx) const
	{
		return m_Children[Idx];
	}

	inline Entity* GetParent() const
	{
		return m_Parent;
	}

	inline uint32 GetChildCount() const
	{
		return static_cast<uint32>(m_Children.size());
	}

	inline uint32 GetComponentCount() const
	{
		return static_cast<uint32>(m_Components.size());
	}

	inline void DestroyComponents(bool bFreeMemory = false)
	{
		for (uint32 i = 0; i < GetComponentCount(); i++)
		{
			GetComponentByIndex(i)->~Component();
		}
		m_Allocator.AllocatedObjects.clear();
		m_Allocator.PtrCurrent = m_Allocator.PtrBegin;
	}

	inline Component* GetComponentByIndex(uint32 Idx)
	{
		BANE_CHECK(Idx < GetComponentCount());
		return reinterpret_cast<Component*>(reinterpret_cast<ptrdiff_t>(m_Allocator.GetAllocatedObjects()[Idx]) + reinterpret_cast<ptrdiff_t>(m_Allocator.PtrBegin));
	}

private:

	friend class Entity;

	Transform m_Transform;
	EntityIdentifier m_Id;
	int32 m_ParentChildIdx;
	Entity* m_Parent;
	std::vector<Entity*> m_Children;
	class Scene* m_SceneOwner;
	ComponentAllocator m_Allocator;
	std::vector<TComponentHandle<Component>> m_ComponentStartList;
	std::vector<uint64> m_Components;
};

Entity* GetEntityById(const EntityIdentifier& Id);

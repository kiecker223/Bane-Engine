#pragma once
#include "Transform.h"
#include "../Components/ComponentBase.h"
#include "../Components/ComponentAllocator.h"
#include "Common/Hash.h"
#include <array>


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



class Entity
{
	typedef std::vector<EntityIdentifier> IdentifierList;
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
	inline T* GetComponent()
	{
		return (T*)GetComponentByHash(T::ClassHash);
	}

	Component* GetComponentByHash(uint64 Hash);

	template<typename T>
	inline T* AddComponent()
	{
		T* RetPointer = m_Allocator.AllocateObject<T>();
		m_Components.push_back(T::ClassHash);
		RetPointer->m_Owner = this;
		RetPointer->m_Transform = &m_Transform;
		RetPointer->Start();
		return RetPointer;
	}

	void SubmitRenderingComponents();

	template<class T, class... U>
	inline T* AddAndConstructComponent(U&&... Params)
	{
		T* RetPointer = m_Allocator.AllocateObjectCtor<T>(Params...);
		m_Components.push_back(T::ClassHash);
		RetPointer->m_Owner = this;
		RetPointer->m_Transform = &m_Transform;
		return RetPointer;
	}

	void RemoveComponent(uint64 ComponentHash);

	template<typename T>
	inline void RemoveComponentByType()
	{
		RemoveComponent(T::ClassHash);
	}

	void AddChild(EntityIdentifier Child);
	void RemoveChild(uint ChildIndex);
	matrix GetMatrixAffectedByParents() const;

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
		for (uint i = 0; i < m_Allocator.GetAllocatedObjects().size(); i++)
			m_Allocator.GetAllocatedObjects()[i]->Start();
	}

	inline void Tick(float DT)
	{
		for (uint i = 0; i < m_Allocator.GetAllocatedObjects().size(); i++)
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

private:

	friend class Entity;

	Entity* GetParent() const;
	
	Transform m_Transform;
	EntityIdentifier m_Id;
	EntityIdentifier m_Parent;
	IdentifierList m_Children;
	
	ComponentAllocator m_Allocator;
	std::vector<uint64> m_Components;
};

Entity* GetEntityById(const EntityIdentifier& Id);
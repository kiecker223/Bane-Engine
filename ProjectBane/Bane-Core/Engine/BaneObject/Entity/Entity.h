#pragma once
#include "Transform.h"
#include "ComponentBase.h"
#include "BaneObjectGeneralAllocator.h"
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

	ForceInline EntityIdentifier& operator = (const EntityIdentifier& Other)
	{
		HashedName = Other.HashedName;
		Name = Other.Name;
		return *this;
	}

	ForceInline operator uint64& ()
	{
		return HashedName;
	}

	ForceInline operator uint64& () const
	{
		return HashedName;
	}

	ForceInline bool IsValid() const
	{
		return HashedName != 0;
	}

	ForceInline operator bool() const
	{
		return IsValid();
	}

	static EntityIdentifier FindIdentifierFromHash(uint64 InHash);
	
	mutable uint64 HashedName;
	std::string Name;
};

ForceInline bool operator == (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return Left.HashedName == Right.HashedName;
}

ForceInline bool operator != (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return !(Left == Right);
}

ForceInline bool operator > (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return (Left.HashedName > Right.HashedName);
}

ForceInline bool operator < (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return (Left.HashedName < Right.HashedName);
}

ForceInline bool operator >= (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return (Left.HashedName >= Right.HashedName);
}

ForceInline bool operator <= (const EntityIdentifier& Left, const EntityIdentifier& Right)
{
	return (Left.HashedName <= Right.HashedName);
}



class Entity
{
	typedef std::vector<EntityIdentifier> IdentifierList;
public:

	Entity() :
		m_Allocator(128) // Allocate with 128 bytes
	{
	}

	Entity(EntityIdentifier Id) :
		m_Allocator(128),
		m_Id(Id)
	{
	}

	template<typename T>
	ForceInline T* GetComponent()
	{
		return (T*)GetComponentByHash(T::ClassHash);
	}

	Component* GetComponentByHash(uint64 Hash);

	template<typename T>
	ForceInline T* AddComponent()
	{
		T* RetPointer = m_Allocator.AllocateObject<T>();
		m_Components.push_back(T::ClassHash);
		RetPointer->m_Owner = this;
		return RetPointer;
	}

	void SubmitRenderingComponents();

	template<class T, class... U>
	ForceInline T* AddAndConstructComponent(U&&... Params)
	{
		T* RetPointer = m_Allocator.AllocateObjectCtor<T>(Params...);
		m_Components.push_back(T::ClassHash);
		RetPointer->m_Owner = this;
		return RetPointer;
	}

	void RemoveComponent(uint64 ComponentHash);

	template<typename T>
	ForceInline void RemoveComponentByType()
	{
		RemoveComponent(T::ClassHash);
	}

	void AddChild(EntityIdentifier Child);
	void RemoveChild(uint ChildIndex);
	XMMATRIX GetMatrixAffectedByParents() const;

	IdentifierList& GetChildren()
	{
		return m_Children;
	}

	EntityIdentifier GetId() const
	{
		return m_Id;
	}

	ForceInline void Start()
	{
		for (uint i = 0; i < m_Allocator.GetAllocatedObjects().size(); i++)
			m_Allocator.GetAllocatedObjects()[i]->Start();
	}

	ForceInline void Tick(double DT)
	{
		for (uint i = 0; i < m_Allocator.GetAllocatedObjects().size(); i++)
			m_Allocator.GetAllocatedObjects()[i]->Tick(DT);
	}

	ForceInline Transform* GetTransform() const
	{
		return (Transform*)&m_Transform;
	}

private:

	friend class Entity;

	Entity* GetParent() const;
	

	Transform m_Transform;
	EntityIdentifier m_Id;
	EntityIdentifier m_Parent;
	IdentifierList m_Children;
	
	BaneObjectGeneralAllocator m_Allocator;
	std::vector<uint64> m_Components;
};
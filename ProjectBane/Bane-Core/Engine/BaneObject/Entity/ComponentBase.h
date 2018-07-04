#pragma once

#include "Core/Common.h"
#include <string>
#include <type_traits>

class Entity;
class Transform;

class Component
{
	friend class Entity;

public:

	virtual ~Component() { }
	
	ForceInline Entity* GetOwner()
	{
		return m_Owner;
	}

	ForceInline Entity* GetOwner() const
	{
		return m_Owner;
	}

	ForceInline Transform* GetTransform()
	{
		return m_Transform;
	}

	ForceInline Transform* GetTransform() const
	{
		return m_Transform;
	}
	
	virtual void Start() { }
	virtual void Tick(float DT) { }
	virtual void Dispose() { }
	virtual void FixedTick() { } // Maybe not do this
	virtual bool IsRenderComponent() const { return false; }

	virtual uint GetTypeSize() const = 0; // Overridden by IMPLEMENT_COMPONENT(type)

private:

	Entity* m_Owner;
	Transform* m_Transform;

};


constexpr const uint64 Component_HashImpl(uint64 InResult, const char* Pointer)
{
	return !*Pointer ? Component_HashImpl((((InResult << 5) + InResult) + *Pointer), Pointer + 1) : InResult;
}


template<typename T>
constexpr const uint64 BuildClassHash()
{
	constexpr uint64 Result = 5381;
	return Component_HashImpl(Result, T::ClassName);
}

#define IMPLEMENT_COMPONENT(x) \
public: \
static constexpr const char* ClassName = #x; \
static constexpr const uint64 ClassHash = BuildClassHash<x>(); \
uint GetTypeSize() const override { return sizeof(x); } \
static uint StaticTypeSize() { return sizeof(x); } \
private:

#define IMPLEMENT_BASE_CLASS(x) \
public: \
static constexpr const uint64 ParentHash = x::ClassHash; \
typedef x ParentType; \
private:



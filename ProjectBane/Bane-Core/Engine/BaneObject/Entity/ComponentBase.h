#pragma once

#include "Core/Common.h"
#include <string>
#include <type_traits>

class Entity;

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
	

	virtual void Start() { }
	virtual void Tick(double DT) { }
	virtual void Dispose() { }
	virtual void FixedTick() { } // Maybe not do this
	virtual bool IsRenderComponent() const { return false; }

	virtual uint GetTypeSize() const = 0; // AHA! A WAY TO MAKE THIS ABSTRACT

private:

	Entity* m_Owner;

};


constexpr const uint64 Component_HashImpl(uint64 InResult, const char* Pointer, uint Index)
{
	return (Pointer[Index] != 0) ? Component_HashImpl((((InResult << 5) + InResult) + Pointer[Index]), Pointer, Index + 1) : InResult;
}


template<typename T>
constexpr const uint64 BuildClassHash()
{
	constexpr uint64 Result = 5381;
	return Component_HashImpl(Result, T::ClassName, 0);
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



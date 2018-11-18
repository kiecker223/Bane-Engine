#pragma once

#include "Common.h"
#include "Common/Hash.h"
#include <string>
#include <type_traits>

class Entity;
class Transform;
class Scene;

class Component
{
	friend class Entity;
public:

	virtual ~Component() { }
	
	inline Entity* GetOwner()
	{
		return m_Owner;
	}

	inline Entity* GetOwner() const
	{
		return m_Owner;
	}

	inline Scene* GetScene()
	{
		return m_Scene;
	}

	inline Transform* GetTransform()
	{
		return m_Transform;
	}

	inline Transform* GetTransform() const
	{
		return m_Transform;
	}
	
	virtual void Awake() { }
	virtual void Start() { }
	virtual void Tick(float DT) { UNUSED(DT); }
	virtual void Dispose() { }
	virtual void FixedTick() { } // Maybe not do this
	virtual bool IsRenderComponent() const { return false; }

	virtual uint32 GetTypeSize() const = 0; // Overridden by IMPLEMENT_COMPONENT(type)

private:

	Entity* m_Owner;
	Scene* m_Scene;
	Transform* m_Transform;

};




#define IMPLEMENT_COMPONENT(x) \
public: \
static constexpr const char* ClassName = #x; \
PRAGMA_STATEMENT(warning(push)) \
PRAGMA_STATEMENT(warning(disable:4307)) \
static constexpr const uint64 ClassHash = GetDJB264BitHash_ConstExpr(#x); \
PRAGMA_STATEMENT(warning(pop)) \
uint32 GetTypeSize() const override { return sizeof(x); } \
static uint32 StaticTypeSize() { return sizeof(x); } \
private:

#define IMPLEMENT_BASE_CLASS(x) \
public: \
static constexpr const uint64 ParentHash = x::ClassHash; \
typedef x ParentType; \
private:



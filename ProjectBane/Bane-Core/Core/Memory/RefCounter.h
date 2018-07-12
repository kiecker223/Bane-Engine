#pragma once

#include "Core/Common.h"
#include <memory>



class NO_VTABLE IRefCounter
{
public:

	virtual void AddRef() = 0;
	virtual void DecrementRef() = 0;
	virtual uint GetRefCount() const = 0;

	virtual void DeleteRef() = 0;
	virtual void DeleteSelf() = 0;
	virtual void AllocateRef() = 0;


	virtual void ReleaseOwnership() = 0;
	
	template<typename... TArgs>
	virtual void ConstructRef(TArgs...&& Args) = 0;

	virtual bool IsValid() const = 0;
};

template<typename T>
class DefaultDeleteAndNullify
{
public:
	explicit DefaultDeleteAndNullify(T** Ptr)
	{
		delete *Ptr;
		*Ptr = nullptr;
	}
};

template<typename T>
class DefaultAllocator
{
public:
	template<typename... TArgs>
	DefaultAllocator(T** OutPtr, TArgs&&... Params)
	{
		*OutPtr = new T(std::forward<TArgs>(TArgs)...);
	}

	DefaultAllocator(T** OutPtr)
	{
		*OutPtr = new T();
	}

};


template<typename TType, typename TAllocator, typename TDeleter>
class OwningRefCounter : public IRefCounter
{
	friend class SharedPointer<typename T, typename TAllocator, typename TDeleter>;
	friend class WeakPointer<typename T, typename TAllocator, typename TDeleter>;
public:

	OwningRefCounter(TType** Original, uint RefCount);

	virtual void AddRef() override;
	virtual void DecrementRef() override;
	virtual uint GetRefCount() const override;
	virtual void DeleteRef() override;
	virtual void DeleteSelf() override;
	virtual void AllocateRef() override;
	virtual void ReleaseOwnership() override;

	template<typename... TArgs>
	virtual void ConstructRef(TArgs...&& Args);

	virtual bool IsValid() const override;

private:

	TType** m_Ptr;
	uint m_RefCount;
};


template<typename TType, typename TSelfDeleter>
class WeakRefCounter : public IRefCounter
{
	friend class SharedPointer<typename T, typename TAllocator, typename TDeleter>;
	friend class WeakPointer<typename T, typename TAllocator, typename TDeleter>;
public:

	explicit WeakRefCounter(TType** Pointer, uint RefCount);

	virtual void AddRef() override;
	virtual void DecrementRef() override;
	virtual uint GetRefCount() const override;
	virtual void DeleteRef();

	// Nothing in these
	virtual void AllocateRef() override {  }

	virtual void ReleaseOwnership() override;
	
	template<typename... TArgs>
	virtual void AllocateRef(TArgs...&& Args) override {  }
	virtual bool IsValid() const override;
	
private:
	
	TType** m_Ptr;
	uint m_RefCount;
};


#include "RefCounter.inl"


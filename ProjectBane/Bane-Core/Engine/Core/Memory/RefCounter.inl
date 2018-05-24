#include "RefCounter.h"


template<typename TType, typename TAllocator, typename TDeleter>
OwningRefCounter<TType, TAllocator, TDeleter>::OwningRefCounter(TType** Original, uint RefCount) :
	m_Ptr(Original),
	m_RefCount(RefCount)
{
}

template<typename TType, typename TAllocator, typename TDeleter>
void OwningRefCounter<TType, TAllocator, TDeleter>::AddRef()
{
	m_RefCount++;
}

template<typename TType, typename TAllocator, typename TDeleter>
void OwningRefCounter<TType, TAllocator, TDeleter>::DecrementRef()
{
	m_RefCount--;
}

template<typename TType, typename TAllocator, typename TDeleter>
uint OwningRefCounter<TType, TAllocator, TDeleter>::GetRefCount() const
{
	return m_RefCount;
}

template<typename TType, typename TAllocator, typename TDeleter>
void OwningRefCounter<TType, TAllocator, TDeleter>::DeleteRef()
{
	TDeleter(m_Ptr);
}

template<typename TType, typename TAllocator, typename TDeleter>
void OwningRefCounter<TType, TAllocator, TDeleter>::DeleteSelf()
{
	TDeleter(&this);
}

template<typename TType, typename TAllocator, typename TDeleter>
void OwningRefCounter<TType, TAllocator, TDeleter>::AllocateRef()
{
	TAllocator(m_Ptr);
}

template<typename TType, typename TAllocator, typename TDeleter>
void OwningRefCounter<TType, TAllocator, TDeleter>::ReleaseOwnership()
{
	m_Ptr = nullptr;
}

template<typename TType, typename TAllocator, typename TDeleter>
template<typename... TArgs>
void OwningRefCounter<TType, TAllocator, TDeleter>::ConstructRef(TArgs...&& Args)
{
	TAllocator(m_Ptr, Args);
}

template<typename TType, typename TAllocator, typename TDeleter>
bool OwningRefCounter<TType, TAllocator, TDeleter>::IsValid() const 
{
	return m_Ptr != nullptr;
}

template<typename TType, typename TSelfDeleter>
WeakRefCounter<TSelfDeleter>::WeakRefCounter(TType* Pointer, uint RefCount) :
	m_RefCount(RefCount)
{
}

template<typename TType, typename TSelfDeleter>
void WeakRefCounter<TSelfDeleter>::AddRef()
{
	m_RefCount++;
}

template<typename TType, typename TSelfDeleter>
void WeakRefCounter<TSelfDeleter>::DecrementRef()
{
	m_RefCount--;
}

template<typename TType, typename TSelfDeleter>
uint WeakRefCounter<TSelfDeleter>::GetRefCount() const
{
	return m_RefCount;
}

template<typename TType, typename TSelfDeleter>
void WeakRefCounter::DeleteRef()
{
	m_Ptr = nullptr;
}

template<typename TType, typename TSelfDeleter>
void WeakRefCounter<TSelfDeleter>::ReleaseOwnership()
{
	m_Ptr = nullptr;
}

template<typename TSelfDeleter>
bool WeakRefCounter<TSelfDeleter>::IsValid() const 
{
	return m_Ptr != nullptr
}


#include "SmartPtr.h"

template<typename T, typename TAllocator, typename TDeleter>
SharedPointer<T, TAllocator, TDeleter>::SharedPointer()
{
	m_RefCount = 0;
	m_Ptr = (T*)0;
}

template<typename T, typename TAllocator, typename TDeleter>
SharedPointer<T, TAllocator, TDeleter>::SharedPointer(T*& ptr)
{
	m_RefCount = 0;
	m_Ptr = ptr;
}

template<typename T, typename TAllocator, typename TDeleter>
SharedPointer<T, TAllocator, TDeleter>::SharedPointer(SharedPointer& Other)
{
	m_RefCounter = Other.GetRefCounter();
}

template<typename T, typename TAllocator, typename TDeleter>
SharedPointer<T, TAllocator, TDeleter>::~SharedPointer()
{
	m_RefCounter->DecrementRef();

	if (GetRefCount() == 0)
	{
		
	}
}

template<typename T, typename TAllocator, typename TDeleter>
uint SharedPointer<T, TAllocator, TDeleter>::GetRefCount() const
{
	return m_RefCount;
}

template<typename T, typename TAllocator, typename TDeleter>
IRefCounter* SharedPointer<T, TAllocator, TDeleter>::GetRefCounter()
{
	return m_RefCounter;
}

template<typename T, typename TAllocator, typename TDeleter>
T*& SharedPointer<T, TAllocator, TDeleter>::Get()
{
	return m_Ptr;
}

template<typename T, typename TAllocator, typename TDeleter>
T*& SharedPointer<T, TAllocator, TDeleter>::Get() const
{
	return m_Ptr;
}

template<typename T, typename TAllocator, typename TDeleter>
void SharedPointer<T, TAllocator, TDeleter>::Free()
{
	delete m_Ptr;
}

template<typename T, typename TAllocator, typename TDeleter>
bool SharedPointer<T, TAllocator, TDeleter>::IsValid() const
{
	return m_Ptr != nullptr;
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::Reset()
{
	
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::Reset(T* Pointer)
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::Reset(SharedPointer<T, TAllocator, TDeleter>& Other)
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::Swap(SharedPointer<T, TAllocator, TDeleter>& Other)
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::ResetPointer()
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::ResetPointer(SharedPointer<T>& Other)
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::ResetPointer(T* Other)
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::ResetPointer(T* Other, uint RefCount)
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::ResetWeak()
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::ResetWeak(SharedPointer<T>& Other)
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline void SharedPointer<T, TAllocator, TDeleter>::ResetWeak(T* Other)
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline T& SharedPointer<T, TAllocator, TDeleter>::operator*()
{
	// TODO: insert return statement here
}

template<typename T, typename TAllocator, typename TDeleter>
inline T& SharedPointer<T, TAllocator, TDeleter>::operator*() const
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline SharedPointer<T, TAllocator, TDeleter>::operator T*&()
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline SharedPointer<T, TAllocator, TDeleter>::operator T*&() const
{
}

template<typename T, typename TAllocator, typename TDeleter>
inline T **& SharedPointer<T, TAllocator, TDeleter>::operator&()
{
	// TODO: insert return statement here
}

template<typename T, typename TAllocator, typename TDeleter>
inline T **& SharedPointer<T, TAllocator, TDeleter>::operator&() const
{
	// TODO: insert return statement here
}

template<typename T, typename TAllocator, typename TDeleter>
inline T*& SharedPointer<T, TAllocator, TDeleter>::operator->()
{
	// TODO: insert return statement here
}

template<typename T, typename TAllocator, typename TDeleter>
inline T*& SharedPointer<T, TAllocator, TDeleter>::operator->() const
{
	// TODO: insert return statement here
}

template<typename T, typename TAllocator, typename TDeleter>
SharedPointer& SharedPointer<T, TAllocator, TDeleter>::operator = (const SharedPointer<T>& right)
{
	return *this;
}

template<typename T, typename TAllocator, typename TDeleter>
SharedPointer& SharedPointer<T, TAllocator, TDeleter>::operator = (const T* right)
{
}







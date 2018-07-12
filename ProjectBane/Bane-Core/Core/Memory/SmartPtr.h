#pragma once

#include "Allocator.h"
#include "RefCounter.h"
#include <memory>

template<typename T, typename TAllocator = DefaultAllocator<T>, typename TDeleter = DefaultDelete<T>>
class WeakPointer
{
public:



};

template<typename T, typename TAllocator = DefaultAllocator<T>, typename TDeleter = DefaultDelete<T>>
class SharedPointer
{
public:
	SharedPointer();
	SharedPointer(SharedPointer& other);
	SharedPointer(T*& ptr);
	~SharedPointer();

	inline uint GetRefCount() const;
	inline IRefCounter* GetRefCounter();

	inline T*& Get();
	inline T*& Get() const;

	inline void Free();
	inline bool IsValid() const;

	inline void Reset();
	inline void Reset(T* Pointer);
	inline void Reset(SharedPointer<T, TAllocator, TDeleter>& Other);
	inline void Swap(SharedPointer<T, TAllocator, TDeleter>& Other);

	inline void ResetPointer();
	inline void ResetPointer(SharedPointer<T, TAllocator, TDeleter>& Other);
	inline void ResetPointer(T* Other);
	inline void ResetPointer(T* Other, uint RefCount);

	inline void ResetWeak();
	inline void ResetWeak(SharedPointer<T, TAllocator, TDeleter>& Other);
	inline void ResetWeak(T* Other);

	inline T& operator* ();
	inline T& operator* () const;

	inline operator T*& ();
	inline operator T*& () const;

	inline T**& operator& ();
	inline T**& operator& () const;

	inline T*& operator-> ();
	inline T*& operator-> () const;

	inline SharedPointer& operator = (const SharedPointer<T, TAllocator, TDeleter>& right);
	inline SharedPointer& operator = (const T* right);
	inline SharedPointer(const SharedPointer& OtherPointer);
		
	template<class TType>
	friend SharedPointer<TType> MakeSharedPointer();

	template<class TType, typename... TArgs>
	friend SharedPointer<TType> MakeSharedPointer(TArgs...&& Args);

private:

	IRefCounter* m_RefCounter;

};


#include "SmartPtr.inl"
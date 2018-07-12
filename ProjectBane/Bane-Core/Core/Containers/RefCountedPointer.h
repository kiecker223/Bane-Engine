#pragma once
#include "Core/Common.h"


template<typename T>
class TRefCountPtr
{
	inline ulong InternalRelease()
	{
		ulong ref = 0;
		if (Ptr)
		{
			ref = Ptr->Release();
		}
		return ref;
	}

	inline ulong InternalAddRef()
	{
		ulong ref = 0;
		if (Ptr)
		{
			ref = Ptr->AddRef();
		}
		if (ref == 0)
		{
			Ptr = nullptr;
		}
		return ref;
	}

public:

	TRefCountPtr() :
		Ptr(nullptr)
	{
	}

	TRefCountPtr(T* InPtr)
	{
		InternalRelease();
		Ptr = InPtr;
		InternalAddRef();
	}

	TRefCountPtr(TRefCountPtr<T>& Other)
	{
		InternalRelease();
		Ptr = Other.Ptr;
		InternalAddRef();
	}

	inline T& operator *()
	{
		return *Ptr;
	}

	inline T& operator *() const
	{
		return *Ptr;
	}

	inline T& operator ->()
	{
		return *Ptr;
	}

	inline T& operator ->() const
	{
		return *Ptr;
	}

	inline operator T*()
	{
		return Ptr;
	}

	inline operator T*() const
	{
		return Ptr;
	}

	inline T** operator &()
	{
		return &Ptr;
	}
	
	inline T** ReleaseAndGetAddressOf()
	{
		Ptr->Release();
		return &Ptr;
	}

	T* Ptr;

};
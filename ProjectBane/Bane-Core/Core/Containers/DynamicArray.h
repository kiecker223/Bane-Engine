#pragma once

#include "Core/Common.h"
#include <Windows.h>


#ifndef AllocFunc
#define AllocFunc(x) HeapAlloc(GetProcessHeap(), 0, x);
#endif

#ifndef FreeFunc
#define FreeFunc(x) HeapFree(GetProcessHeap(), 0, x);
#endif

template<typename T>
class DynamicArray
{
public:

	void Reserve(uint Count);


	T* GetData();

private:

	T* m_Begin;
	ushort m_Size;
	ushort m_Reserved;

};

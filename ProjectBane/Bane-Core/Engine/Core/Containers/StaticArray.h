#pragma once

#include "Common.h"
#include <array>
#include "CIterator.h"
#include <initializer_list>

template<typename T, uint SIZE = 32>
class StaticArray
{
public:

	typedef CIterator<T> IteratorType;

	StaticArray();
	StaticArray(const StaticArray<T, SIZE>& other);
	StaticArray(std::initializer_list<T> initList);
	StaticArray(T* data, uint dataSize);
	
	inline void Append(const T& value);
	inline void PopBack();
	
	inline IteratorType Begin();
	inline IteratorType Begin() const;
					    
	inline IteratorType End();
	inline IteratorType End() const;

	inline uint GetUsed() const { return m_Used; }

	inline T& operator[](uint pos);
	inline T& operator[](uint pos) const;

private:

	uint	m_Used;
	T		m_StoredArray[SIZE];
};

#include "StaticArray.inl"

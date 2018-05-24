#pragma once

#include "DynamicArray.h"
#include "Pair.h"

template<typename Key, typename Value>
class MapArray
{
public:

	inline void Add(const Key& key, const Value& data);
	inline void Add(const Pair<Key, Value>& value);
	
	inline Value& Find(const Key& value);
	inline Value& Find(const Key& value) const;

	inline Value& operator[](const Key& key);
	inline Value& operator[](const Key& key) const;

	inline bool Contains(const Key& key) const;

	inline Value& IndexOf(uint idx);
	inline Value& IndexOf(uint idx) const;

	inline void RemoveValue(const Key& key);

private:

	DynamicArray<Key>	m_Keys;
	DynamicArray<Value> m_Values;

};


#include "MapArray.inl"

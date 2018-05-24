
template<typename T, uint SIZE>
StaticArray<T, SIZE>::StaticArray() :
	m_Used(1)
{
}

template<typename T, uint SIZE>
StaticArray<T, SIZE>::StaticArray(const StaticArray<T, SIZE>& other) :
	m_Used(1)
{
	for (uint i = 0; i < other.GetUsed(); i++)
	{
		m_StoredArray[i] = other[i];
	}
}

template<typename T, uint SIZE>
StaticArray<T, SIZE>::StaticArray(std::initializer_list<T> initList) :
	m_Used(1)
{
	for (uint i = 0, std::initializer_list<T>::iterator it = initList.begin(); it < initList.end(); it++, i++)
	{
		m_StoredArray[i] = *it;
	}
}

template<typename T, uint SIZE>
StaticArray<T, SIZE>::StaticArray(T* data, uint dataSize) :
	m_Used(1)
{
	for (uint i = 0; i < dataSize; i++)
	{
		m_StoredArray[i] = data[i];
	}
}

template<typename T, uint SIZE>
void StaticArray<T, SIZE>::Append(const T& value)
{
	m_StoredArray[m_Used - 1] = std::move(value);
	m_Used++;
}

template<typename T, uint SIZE>
void StaticArray<T, SIZE>::PopBack()
{
	m_StoredArray[m_Used - 1].~T();
	m_Used--;
}

template<typename T, uint SIZE>
IteratorType StaticArray<T, SIZE>::Begin()
{
	CIterator<T> it(&m_StoredArray[0]);
	return it;
}

template<typename T, uint SIZE>
IteratorType StaticArray<T, SIZE>::Begin() const
{
	CIterator<T> it(&m_StoredArray[0]);
	return it;
}

template<typename T, uint SIZE>
IteratorType StaticArray<T, SIZE>::End()
{
	CIterator<T> it(&m_StoredArray[m_Used - 1]);
	return it;
}

template<typename T, uint SIZE>
IteratorType StaticArray<T, SIZE>::End() const
{
	CIterator<T> it(&m_StoredArray[m_Used - 1]);
	return it;
}

template<typename T, uint SIZE>
T& StaticArray<T, SIZE>::operator[](uint pos)
{
	return m_StoredArray[pos];
}

template<typename T, uint SIZE>
T& StaticArray<T, SIZE>::operator[](uint pos) const
{
	return m_StoredArray[pos];
}


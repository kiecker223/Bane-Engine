


template<typename Key, typename Value>
void MapArray<Key, Value>::Add(const Key& key, const Value& data)
{
	m_Keys.Append(key);
	m_Values.Append(data);
}

template<typename Key, typename Value>
void MapArray<Key, Value>::Add(const Pair<Key, Value>& value)
{
	m_Keys.Append(value.first);
	m_Values.Append(value.second);
}

template<typename Key, typename Value>
Value& MapArray<Key, Value>::Find(const Key& value)
{
	for (uint i = 0; i < m_Keys.GetSize(); i++)
	{
		if (m_Keys[i] == value)
		{
			return m_Values[i];
		}
	}
}

template<typename Key, typename Value>
Value& MapArray<Key, Value>::Find(const Key& value) const
{
	for (uint i = 0; i < m_Keys.GetSize(); i++)
	{
		if (m_Keys[i] == value)
		{
			return m_Values[i];
		}
	}
}

template<typename Key, typename Value>
Value& MapArray<Key, Value>::operator[](const Key& key)
{
	return Find(key);
}

template<typename Key, typename Value>
Value& MapArray<Key, Value>::operator[](const Key& key) const
{
	return Find(key);
}


template<typename Key, typename Value>
bool MapArray<Key, Value>::Contains(const Key& key) const 
{
	for (uint i = 0; i < m_Keys.GetSize(); i++)
	{
		if (m_Keys[i] == value)
		{
			return true;
		}
	}

	return false;
}

template<typename Key, typename Value>
Value& MapArray<Key, Value>::IndexOf(uint idx)
{
	return m_Values[idx];
}

template<typename Key, typename Value>
Value& MapArray<Key, Value>::IndexOf(uint idx) const
{
	return m_Values[idx];
}

template<typename Key, typename Value>
void MapArray<Key, Value>::RemoveValue(const Key& key)
{
	int foundIdx = -1;

	for (uint i = 0; i < m_Keys.GetSize(); i++)
	{
		if (m_Keys[i] == key)
		{
			foundIdx = (int)i;
		}
	}

	if (foundIdx == -1)
		return;

	m_Keys.RemoveAt(foundIdx);
	m_Values.RemoveAt(foundIdx);
}



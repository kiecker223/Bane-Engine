#pragma once
#include <iterator>
#include <vector>

template<typename TType>
class TArray
{
public:

	void Add(TType&& Value)
	{

	}

	void Add(const TType& Value)
	{
		return Add(std::move(Value));
	}

	inline void RemoveAt(uint32 Index) 
	{
		
	}

	class TForwardIterator : public std::iterator
	{

	};

	class TBackwardIterator : public std::iterator
	{

	};

	inline TForwardIterator begin()
	{
		return TForwardIterator();
	}

private:

	TType* m_Start;
	TType* m_Current;
	TType* m_End;
};


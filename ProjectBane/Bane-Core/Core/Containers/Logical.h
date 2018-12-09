#pragma once


template<class T>
class CompareLess
{
public:
	static bool Compare(const T& Lhs, const T& Rhs) const
	{
		return Lhs < Rhs;
	}
};

template<class T>
class CompareGreater
{
public:
	static bool Compare(const T& Lhs, const T& Rhs) const 
	{
		return Lhs > Rhs;
	}
};



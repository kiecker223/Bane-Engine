#pragma once



template<typename T1, typename T2>
struct Pair
{
public:

	inline Pair(T1& t1, T2& t2) : first(t1), second(t2) { }

	T1& first;
	T2& second;
	
};
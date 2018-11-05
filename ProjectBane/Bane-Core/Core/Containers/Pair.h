#pragma once



template<typename T1, typename T2>
struct Pair
{
public:

	Pair(T1& t1, T2& t2) : First(t1), Second(t2) { }
	T1 First;
	T2 Second;
	
};
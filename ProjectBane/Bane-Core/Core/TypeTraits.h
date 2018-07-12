#pragma once

#include <type_traits>

template<class TType, TType TValue>
class IntegralConstant
{
public:
	static constexpr TType Value = TValue;

	typedef TType ValueType;
	typedef IntegralConstant<TType, TValue> Type;

	constexpr operator ValueType() const 
	{	
		return (Value);
	}

	constexpr ValueType operator()() const 
	{
		return (Value);
	}
};


typedef IntegralConstant<bool, true> TrueType;
typedef IntegralConstant<bool, false> FalseType;


template<typename T1, typename T2>
class TAreTypesSame 
{
public:
	typedef FalseType ValueType;
};

template<typename T1>
class TAreSameType<T1, T1>
{
public:
	typedef TrueType ValueType;
};

template<class T1, class T2>
inline bool AreTypesSame(T1& Left, T2& Right) const
{
	return TAreTypesSame<T1, T2>::ValueType::Value;
}

template<class T1, class T2>
inline bool AreTypesSame() const
{
	return TAreTypesSame<T1, T2>::ValueType::Value;
}

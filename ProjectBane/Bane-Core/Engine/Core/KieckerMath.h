#pragma once
#ifndef __BANE_MATH_H__
#define __BANE_MATH_H__

#include "Core/Common.h"


#define _PI_ 3.141592654f
#define _M_PI_ 3.14159265358979323846

// Disable anonymous structures warnings
#pragma warning(disable:4201)

inline uint NextPowerOfTwo(uint Value)
{
	Value--;
	Value |= Value >> 1;
	Value |= Value >> 2;
	Value |= Value >> 4;
	Value |= Value >> 8;
	Value |= Value >> 16;
	Value++;
	return Value;
}

template<class T>
inline float length(const T& InType);

inline float radians(float Degrees)
{
	return (_PI_ / 180.f) * Degrees;
}

inline float degrees(float Radians)
{
	return (180.f / _PI_) * Radians;
}

template<typename T, uint TColCount>
struct TSingleRowMathStruct
{
	static const uint ColCount = TColCount;
	using TType = T;
	T p[TColCount];

	inline T& operator[](const uint Index)
	{
		return p[Index];
	}

	inline const T operator[](const uint Index) const
	{
		return p[Index];
	}
};


template<typename T>
struct TSingleRowMathStruct<T, 2>
{
	using TType = T;
	//using TT = TSingleRowMathStruct<T, 2>;
	typedef TSingleRowMathStruct<T, 2> TT;
	static const uint ColCount = 2;
	TSingleRowMathStruct()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	TSingleRowMathStruct(T X, T Y) :
		x(X),
		y(Y)
	{
	}

	TSingleRowMathStruct(const TT& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}
	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
		T p[ColCount];
	};
	inline TSingleRowMathStruct<T, 2>& operator = (const TSingleRowMathStruct<T, 2>& Rhs)
	{
		for (uint i = 0; i < TSingleRowMathStruct<T, 2>::ColCount; i++)
		{
			p[i] = Rhs[i];
		}
		return *this;
	}
	inline T& operator[] (const uint Index)
	{
		return p[Index];
	}

	inline const T operator[] (const uint Index) const
	{
		return p[Index];
	}

#pragma region Operators

#pragma region Self modifyable operators

	inline TT& operator *= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs[i];
		}
		return *this;
	}

	inline TT& operator *= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs;
		}
		return *this;
	}

	inline TT& operator /= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs;
		}
		return *this;
	}

	inline TT& operator += (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs;
		}
		return *this;
	}

	inline TT& operator -= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs;
		}
		return *this;
	}

#pragma endregion

#pragma region Non self modifying operators

	inline TT operator-() const
	{
		return TT(-x, -y);
	}

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs[i];
		}
		return Result;
	}

	inline TT operator * (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs;
		}
		return Result;
	}

	inline TT operator / (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs;
		}
		return Result;
	}

	inline TT operator + (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs;
		}
		return Result;
	}

	inline TT operator - (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs;
		}
		return Result;
	}

	inline bool operator > (const T& Rhs)
	{
		return length(*this) > length(Rhs);
	}

	inline bool operator < (const T& Rhs)
	{
		return length(*this) < length(Rhs);
	}

	inline bool operator == (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			if (p[i] != Rhs[i])
			{
				return false;
			}
		}
		return true;
	}
#pragma endregion

#pragma endregion
};

template<typename T>
struct TSingleRowMathStruct<T, 3>
{
	using TType = T;
	using TT = TSingleRowMathStruct<T, 3>;
	using TT2 = TSingleRowMathStruct<T, 2>;
	static const uint ColCount = 3;
	TSingleRowMathStruct()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	TSingleRowMathStruct(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	TSingleRowMathStruct(const TT& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	TSingleRowMathStruct(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	TSingleRowMathStruct(TT2 XY, T Z) :
		x(XY.x),
		y(XY.y),
		z(Z)
	{
	}

	union
	{
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { T u, v, w; };
		TT2 rg;
		TT2 xy;
		T p[ColCount];
	};
	inline TSingleRowMathStruct<T, 3>& operator = (const TSingleRowMathStruct<T, 3>& Rhs)
	{
		for (uint i = 0; i < TSingleRowMathStruct<T, 3>::ColCount; i++)
		{
			p[i] = Rhs[i];
		}
		return *this;
	}
	inline T& operator[] (const uint Index)
	{
		return p[Index];
	}

	inline const T operator[] (const uint Index) const
	{
		return p[Index];
	}

#pragma region Operators

#pragma region Self modifyable operators


	inline TT& operator *= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs[i];
		}
		return *this;
	}

	inline TT& operator *= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs;
		}
		return *this;
	}

	inline TT& operator /= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs;
		}
		return *this;
	}

	inline TT& operator += (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs;
		}
		return *this;
	}

	inline TT& operator -= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs;
		}
		return *this;
	}

#pragma endregion

#pragma region Non self modifying operators

	inline TT operator -() const
	{
		return TT(-x, -y, -z);
	}

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs[i];
		}
		return Result;
	}

	inline TT operator * (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs;
		}
		return Result;
	}

	inline TT operator / (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs;
		}
		return Result;
	}

	inline TT operator + (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs;
		}
		return Result;
	}

	inline TT operator - (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs;
		}
		return Result;
	}


	inline bool operator > (const T& Rhs)
	{
		return length(*this) > length(Rhs);
	}

	inline bool operator < (const T& Rhs)
	{
		return length(*this) < length(Rhs);
	}

	inline bool operator == (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			if (p[i] != Rhs[i])
			{
				return false;
			}
		}
		return true;
	}

#pragma endregion

#pragma endregion
};

template<typename T>
struct TSingleRowMathStruct<T, 4>
{
	using TType = T;
	using TT = TSingleRowMathStruct<T, 4>;
	using TT3 = TSingleRowMathStruct<T, 3>;
	using TT2 = TSingleRowMathStruct<T, 2>;
	static const uint ColCount = 4;
	TSingleRowMathStruct()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	TSingleRowMathStruct(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	TSingleRowMathStruct(const TT& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	TSingleRowMathStruct(T X, T Y, TT2 ZW) :
		x(X),
		y(Y),
		z(ZW.x),
		y(ZW.y)
	{
	}

	TSingleRowMathStruct(TT3 XYZ, T W) :
		x(XYZ.x),
		y(XYZ.y),
		z(XYZ.w),
		w(W)
	{
	}

	union
	{
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		struct { T u, v, w, P0; };
		TT3 xyz;
		TT3 rgb;
		TT3 uvw;
		struct
		{
			TT2 xy, zw;
		};
		struct
		{
			TT2 rg, ba;
		};
		struct
		{
			TT2 uv, wz;
		};
		T p[ColCount];
	};

	inline TSingleRowMathStruct<T, 4>& operator = (const TSingleRowMathStruct<T, 4>& Rhs)
	{
		for (uint i = 0; i < TT::ColCount; i++)
		{
			p[i] = Rhs[i];
		}
		return *this;
	}

	inline T& operator[] (const uint Index)
	{
		return p[Index];
	}

	inline const T operator[] (const uint Index) const
	{
		return p[Index];
	}

#pragma region Operators

#pragma region Self modifyable operators

	inline TT& operator *= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs[i];
		}
		return *this;
	}

	inline TT& operator *= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs;
		}
		return *this;
	}

	inline TT& operator /= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs;
		}
		return *this;
	}

	inline TT& operator += (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs;
		}
		return *this;
	}

	inline TT& operator -= (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs;
		}
		return *this;
	}

#pragma endregion

#pragma region Non self modifying operators

	inline TT operator -() const
	{
		return TT(-x, -y, -z, -w);
	}

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs[i];
		}
		return Result;
	}

	inline TT operator * (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs;
		}
		return Result;
	}

	inline TT operator / (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs;
		}
		return Result;
	}

	inline TT operator + (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs;
		}
		return Result;
	}

	inline TT operator - (const T& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs;
		}
		return Result;
	}

	inline bool operator > (const T& Rhs)
	{
		return length(*this) > length(Rhs);
	}

	inline bool operator < (const T& Rhs)
	{
		return length(*this) < length(Rhs);
	}

	inline bool operator == (const T& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			if (p[i] != Rhs[i])
			{
				return false;
			}
		}
		return true;
	}

#pragma endregion

#pragma endregion
};


template<class T>
inline float length(const T& InType)
{
	float Result = 0;
	for (uint i = 0; i < T::ColCount; i++)
	{
		Result += (InType[i] * InType[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

template<class T>
inline T normalized(const T& InType)
{
	return InType / length(InType);
}

template<class T>
inline T& normalize(T& InType)
{
	return InType /= length(InType);
}

template<class T>
inline float dot(const T& Lhs, const T& Rhs)
{
	float Result = 0;
	for (uint i = 0; i < T::ColCount; i++)
	{
		Result += (Lhs[i] * Rhs[i]);
	}
	return Result;
}

template<class T>
inline T reflect(const T& Incidence, const T& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

template<class T>
inline T refract(const T& Incidence, const T& Normal, float IndicesToRefraction)
{
	float NDotI = dot(Normal, Incidence);
	float K = 1.0f - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0f)
	{
		return float3();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrtf(K)));
}

template<typename T>
inline T lerp(const T& V0, const T& V1, float Factor)
{
	return V0 * (1 - Factor) + V1 * Factor;
}

typedef TSingleRowMathStruct<float, 4> float4;
typedef TSingleRowMathStruct<float, 3> float3;
typedef TSingleRowMathStruct<float, 2> float2;
typedef TSingleRowMathStruct<int, 4> int4;
typedef TSingleRowMathStruct<int, 3> int3;
typedef TSingleRowMathStruct<int, 2> int2;
typedef TSingleRowMathStruct<uint, 4> uint4;
typedef TSingleRowMathStruct<uint, 3> uint3;
typedef TSingleRowMathStruct<uint, 2> uint2;




inline float3 cross(const float3& Lhs, const float3& Rhs)
{
	return float3(
		Lhs.y * Rhs.z - Rhs.y * Lhs.z,
		Lhs.z * Rhs.x - Rhs.z * Lhs.x,
		Lhs.x * Rhs.y - Rhs.x * Lhs.y
	);
}


template<typename T, uint TNumRows, uint TNumCols>
struct TMultiRowColumnMathStruct
{
	using TT = TMultiRowColumnMathStruct<T, TNumRows, TNumCols>;
	using TType = T;
	static const uint NumRows = TNumRows;
	static const uint NumCols = TNumCols;
	using RowType = TSingleRowMathStruct<T, NumCols>;

	union 
	{
		T p[TNumRows][TNumCols];
		RowType v[NumRows];
	};

	inline RowType& operator[](uint Index)
	{
		return v[Index];
	}

	inline RowType operator[](uint Index) const
	{
		return v[Index];
	}
};

template<typename T>
struct TMultiRowColumnMathStruct<T, 2, 2>
{
	using TT = TMultiRowColumnMathStruct<T, 2, 2>;
	using TType = T;
	using RowType = TSingleRowMathStruct<T, 2>;
	static const uint NumRows = 2;
	static const uint NumCols = 2;

	union 
	{
		T p[2][2];
		RowType v[2];
	};
	
	TMultiRowColumnMathStruct()
	{
		v[0] = RowType();
		v[1] = RowType();
	}

	TMultiRowColumnMathStruct(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
	}

	TMultiRowColumnMathStruct(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	TMultiRowColumnMathStruct(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	TMultiRowColumnMathStruct(T p00, T p01, T p10, T p11)
	{
		v[0] = RowType(p00, p01);
		v[1] = RowType(p10, p11);
	}

	TMultiRowColumnMathStruct(RowType r1, RowType r2)
	{
		v[0] = r1;
		v[1] = r2;
	}

	inline RowType& operator[] (const uint Index)
	{
		return v[Index];
	}

	inline const RowType operator[] (const uint Index) const
	{
		return v[Index];
	}

#pragma region Operators
	
	inline TT& operator = (const TT& Rhs)
	{
		memcpy(this, (void*)&Rhs, sizeof(*this));
		return *this;
	}

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		
		Result[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0];
		Result[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1];

		Result[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0];
		Result[1][1] = p[0][1] * Rhs[1][0] + p[1][1] * Rhs[1][1];
		
		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		p[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0];
		p[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1];

		p[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0];
		p[1][1] = p[0][1] * Rhs[1][0] + p[1][1] * Rhs[1][1];
		return *this;
	}

	inline RowType operator * (const RowType& Rhs) const
	{
		RowType Result;
		Result.x = Rhs.x * p[0][0] + Rhs.y * p[1][0];
		Result.y = Rhs.x * p[0][1] + Rhs.y * p[1][1];
		return Result;
	}

	inline TT& operator *= (float Rhs)
	{
		for (uint i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (float Rhs) const
	{
		TT Result;
		for (uint i = 0; i < NumRows; i++)
		{
			Result[i] = v[i] * Rhs;
		}
		return Result;
	}

#pragma endregion
};

template<typename T>
struct TMultiRowColumnMathStruct<T, 3, 3>
{
	using TT = TMultiRowColumnMathStruct<T, 3, 3>;
	using TT2 = TMultiRowColumnMathStruct<T, 2, 2>;
	using TType = T;
	using RowType = TSingleRowMathStruct<T, 3>;
	static const uint NumRows = 3;
	static const uint NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	TMultiRowColumnMathStruct()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	TMultiRowColumnMathStruct(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	TMultiRowColumnMathStruct(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	TMultiRowColumnMathStruct(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	TMultiRowColumnMathStruct(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	TMultiRowColumnMathStruct(RowType r1, RowType r2, RowType r3)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
	}


#pragma region Operators

	inline TT& operator = (const TT& Rhs)
	{
		memcpy(this, (void*)&Rhs, sizeof(*this));
		return *this;
	}


	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		Result[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		Result[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		Result[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		Result[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		Result[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		Result[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		Result[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		Result[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		Result[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		p[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		p[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		p[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];
		
		p[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		p[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		p[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];
		
		p[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		p[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		p[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return *this;
	}

	inline RowType operator * (const RowType& Rhs) const
	{
		RowType Result;
		Result.x = Rhs.x * p[0][0] + Rhs.y * p[0][1] + Rhs.z * p[0][2];
		Result.y = Rhs.x * p[1][0] + Rhs.y * p[1][1] + Rhs.z * p[1][2];
		Result.z = Rhs.x * p[2][0] + Rhs.y * p[2][1] + Rhs.z * p[2][2];
		return Result;
	}

	inline TT& operator *= (float Rhs)
	{
		for (uint i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (float Rhs) const
	{
		TT Result;
		for (uint i = 0; i < NumRows; i++)
		{
			Result[i] = v[i] * Rhs;
		}
		return Result;
	}

	inline operator TT2() const
	{
		return TT2(
			p[0][0], p[0][1],
			p[1][0], p[1][1]
		);
	}

#pragma endregion
};

template<typename T>
struct TMultiRowColumnMathStruct<T, 4, 4>
{
	using TT = TMultiRowColumnMathStruct<T, 4, 4>;
	using TT3 = TMultiRowColumnMathStruct<T, 3, 3>;
	using TT2 = TMultiRowColumnMathStruct<T, 2, 2>;
	using RowType = TSingleRowMathStruct<T, 4>;
	using TType = T;
	static const uint NumRows = 4;
	static const uint NumCols = 4;

	union
	{
		T p[4][4];
		RowType v[4];
	};

	TMultiRowColumnMathStruct()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	TMultiRowColumnMathStruct(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
		p[3][3] = IdentityVal;
	}

	TMultiRowColumnMathStruct(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	TMultiRowColumnMathStruct(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	TMultiRowColumnMathStruct(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	TMultiRowColumnMathStruct(RowType r1, RowType r2, RowType r3, RowType r4)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
		v[3] = r4;
	}

	inline RowType& operator[](uint Index)
	{
		return v[Index];
	}

	inline RowType operator[](uint Index) const
	{
		return v[Index];
	}

#pragma region Operators

	inline TT& operator = (const TT& Rhs)
	{
		memcpy(this, (void*)&Rhs, sizeof(*this));
		return *this;
	}

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		Result[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0] + p[0][3] * Rhs[3][0];
		Result[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1] + p[0][3] * Rhs[3][1];
		Result[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2] + p[0][3] * Rhs[3][2];
		Result[0][3] = p[0][0] * Rhs[0][3] + p[0][1] * Rhs[1][3] + p[0][2] * Rhs[2][3] + p[0][3] * Rhs[3][3];

		Result[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0] + p[1][3] * Rhs[3][0];
		Result[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1] + p[1][3] * Rhs[3][1];
		Result[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2] + p[1][3] * Rhs[3][2];
		Result[1][3] = p[1][0] * Rhs[0][3] + p[1][1] * Rhs[1][3] + p[1][2] * Rhs[2][3] + p[1][3] * Rhs[3][3];

		Result[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0] + p[2][3] * Rhs[3][0];
		Result[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1] + p[2][3] * Rhs[3][1];
		Result[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2] + p[2][3] * Rhs[3][2];
		Result[2][3] = p[2][0] * Rhs[0][3] + p[2][1] * Rhs[1][3] + p[2][2] * Rhs[2][3] + p[2][3] * Rhs[3][3];

		Result[3][0] = p[3][0] * Rhs[0][0] + p[3][1] * Rhs[1][0] + p[3][2] * Rhs[2][0] + p[3][3] * Rhs[3][0];
		Result[3][1] = p[3][0] * Rhs[0][1] + p[3][1] * Rhs[1][1] + p[3][2] * Rhs[2][1] + p[3][3] * Rhs[3][1];
		Result[3][2] = p[3][0] * Rhs[0][2] + p[3][1] * Rhs[1][2] + p[3][2] * Rhs[2][2] + p[3][3] * Rhs[3][2];
		Result[3][3] = p[3][0] * Rhs[0][3] + p[3][1] * Rhs[1][3] + p[3][2] * Rhs[2][3] + p[3][3] * Rhs[3][3];
		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		p[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0] + p[0][3] * Rhs[3][0];
		p[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1] + p[0][3] * Rhs[3][1];
		p[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2] + p[0][3] * Rhs[3][2];
		p[0][3] = p[0][0] * Rhs[0][3] + p[0][1] * Rhs[1][3] + p[0][2] * Rhs[2][3] + p[0][3] * Rhs[3][3];
		
		p[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0] + p[1][3] * Rhs[3][0];
		p[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1] + p[1][3] * Rhs[3][1];
		p[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2] + p[1][3] * Rhs[3][2];
		p[1][3] = p[1][0] * Rhs[0][3] + p[1][1] * Rhs[1][3] + p[1][2] * Rhs[2][3] + p[1][3] * Rhs[3][3];
		
		p[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0] + p[2][3] * Rhs[3][0];
		p[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1] + p[2][3] * Rhs[3][1];
		p[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2] + p[2][3] * Rhs[3][2];
		p[2][3] = p[2][0] * Rhs[0][3] + p[2][1] * Rhs[1][3] + p[2][2] * Rhs[2][3] + p[2][3] * Rhs[3][3];
		
		p[3][0] = p[3][0] * Rhs[0][0] + p[3][1] * Rhs[1][0] + p[3][2] * Rhs[2][0] + p[3][3] * Rhs[3][0];
		p[3][1] = p[3][0] * Rhs[0][1] + p[3][1] * Rhs[1][1] + p[3][2] * Rhs[2][1] + p[3][3] * Rhs[3][1];
		p[3][2] = p[3][0] * Rhs[0][2] + p[3][1] * Rhs[1][2] + p[3][2] * Rhs[2][2] + p[3][3] * Rhs[3][2];
		p[3][3] = p[3][0] * Rhs[0][3] + p[3][1] * Rhs[1][3] + p[3][2] * Rhs[2][3] + p[3][3] * Rhs[3][3];
		return *this;
	}

	inline RowType operator * (const RowType& Rhs) const
	{
		RowType Result;
		Result.x = Rhs.x * p[0][0] + Rhs.y * p[0][1] + Rhs.z * p[0][2] + Rhs.w * p[0][3];
		Result.y = Rhs.x * p[1][0] + Rhs.y * p[1][1] + Rhs.z * p[1][2] + Rhs.w * p[1][3];
		Result.z = Rhs.x * p[2][0] + Rhs.y * p[2][1] + Rhs.z * p[2][2] + Rhs.w * p[2][3];
		Result.w = Rhs.x * p[3][0] + Rhs.y * p[3][1] + Rhs.z * p[3][2] + Rhs.w * p[3][3];
		return Result;
	}

	inline TT& operator *= (float Rhs)
	{
		for (uint i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (float Rhs) const
	{
		TT Result;
		for (uint i = 0; i < NumRows; i++)
		{
			Result[i] = v[i] * Rhs;
		}
		return Result;
	}

	inline operator TT3() const
	{
		return TT3(
			p[0][0], p[0][1], p[0][2],
			p[1][0], p[1][1], p[1][2],
			p[2][0], p[2][1], p[2][2]
		);
	}

	inline operator TT2() const
	{
		return TT2(
			p[0][0], p[0][1],
			p[1][0], p[1][1]
		);
	}

#pragma endregion
};



typedef TMultiRowColumnMathStruct<float, 2, 2> float2x2;
typedef TMultiRowColumnMathStruct<float, 3, 3> float3x3;
typedef TMultiRowColumnMathStruct<float, 4, 4> float4x4;
typedef float4x4 matrix;
typedef TMultiRowColumnMathStruct<int, 2, 2> int2x2;
typedef TMultiRowColumnMathStruct<int, 3, 3> int3x3;
typedef TMultiRowColumnMathStruct<int, 4, 4> int4x4;
typedef TMultiRowColumnMathStruct<uint, 2, 2> uint2x2;
typedef TMultiRowColumnMathStruct<uint, 3, 3> uint3x3;
typedef TMultiRowColumnMathStruct<uint, 4, 4> uint4x4;


struct Quaternion
{
public:

	float w, x, y, z;

	Quaternion()
	{
	}

	Quaternion(const Quaternion& Rhs) :
		w(Rhs.w),
		x(Rhs.x),
		y(Rhs.y),
		z(Rhs.z)
	{
	}

	Quaternion(float W, float X, float Y, float Z) :
		w(W),
		x(X),
		y(Y),
		z(Z)
	{
	}

	Quaternion(const float3& Euler)
	{
		*this = ((Quaternion(float3(1.f, 0.f, 0.f), radians(Euler.x)) * Quaternion(float3(0.f, 1.f, 0.f), radians(Euler.y)) * Quaternion(float3(0.f, 0.f, 1.f), radians(Euler.z))).Normalized());
	}

	Quaternion(const float3& Axis, const float Angle)
	{
		*this = Quaternion::FromAxisAngle(Axis, Angle);
	}

	inline Quaternion& operator = (const Quaternion& Rhs)
	{
		w = Rhs.w;
		x = Rhs.x;
		y = Rhs.y;
		z = Rhs.z;
		return *this;
	}

	inline static Quaternion FromAxisAngle(const float3& InAxis, const float Angle)
	{
		float3 Axis = InAxis;
		Quaternion Result;
		const float s = sinf(Angle / 2);
		Result.x = Axis.x * s;
		Result.y = Axis.y * s;
		Result.z = Axis.z * s;
		Result.w = cos(Angle / 2);
		Result.Normalize(); // The length is always 1
		return Result;
	}

	inline Quaternion Conjugate()
	{
		return Quaternion(-x, -y, -z, w);
	}

	inline Quaternion operator * (const Quaternion& Rhs) const
	{
		Quaternion Result;

		Result.w = (w * Rhs.w - x * Rhs.x - y * Rhs.y - z * Rhs.z);
		Result.x = (w * Rhs.x + x * Rhs.w - y * Rhs.z + z * Rhs.y);
		Result.y = (w * Rhs.y + x * Rhs.z + y * Rhs.w - z * Rhs.x);
		Result.z = (w * Rhs.z - x * Rhs.y + y * Rhs.x + z * Rhs.w);

		return Result;
	}

	inline Quaternion& operator *= (const Quaternion& Rhs)
	{
		w = (w * Rhs.w - x * Rhs.x - y * Rhs.y - z * Rhs.z);
		x = (w * Rhs.x + x * Rhs.w - y * Rhs.z + z * Rhs.y);
		y = (w * Rhs.y + x * Rhs.z + y * Rhs.w - z * Rhs.x);
		z = (w * Rhs.z - x * Rhs.y + y * Rhs.x + z * Rhs.w);
		return *this;
	}

	inline float3 GetForward() const
	{
		return float3(2.f * (x * z - w * y), 2.f * (y * z + w * z), 1.f - 2.f * (x * x + y * y));
	}

	inline float3 GetBack() const
	{
		return -GetForward();
	}

	inline float3 GetUp() const
	{
		return float3(2.f * (x * y + w * z), 1.f - 2.f * (x * x + z * z), 2.f * (y * z - w * x));
	}

	inline float3 GetDown() const
	{
		return -GetUp();
	}

	inline float3 GetRight() const
	{
		return float3(1.f - 2.f * (y * y + z * z), 2.f * (x * y - w * z), 2.f * (x * z + w * y));
	}

	inline float3 GetLeft() const
	{
		return -GetRight();
	}

	inline matrix RotationMatrix() const
	{
		const float3 R = GetRight();
		const float3 U = GetUp();
		const float3 F = GetForward();
		matrix Result(
			float4(R, 0.f),
			float4(U, 0.f),
			float4(F, 0.f),
			float4(0.f, 0.f, 0.f, 1.f)
		);
		return Result;
	}

	inline Quaternion Normalized() const
	{
		Quaternion q(*this);
		float Length = sqrt(w * w + x * x + y * y + z * z);
		q.w /= Length;
		q.x /= Length;
		q.y /= Length;
		q.z /= Length;
		return q;
	}

	inline Quaternion& Normalize()
	{
		float Length = sqrt(w * w + x * x + y * y + z * z);
		w /= Length;
		x /= Length;
		y /= Length;
		z /= Length;
		return *this;
	}
};

inline matrix matProjection(float Aspect, float FovY, float Near, float Far)
{
	FovY = radians(FovY);
	float TanFovOverTwo = tanf(FovY / 2.f);
	matrix Result(
		1.f / (Aspect * TanFovOverTwo),	0.f,					0.f,							0.f,
		0.f,							1.f / TanFovOverTwo,	0.f,							0.f,
		0.f,							0.f,					-((Far + Near) / (Far - Near)), -((2 * Far * Near) / (Far - Near)),
		0.f,							0.f,					-1.f,							0.f
	);
	return Result;
}

template<class T>
T transpose(const T& InMatrix)
{
	T Result;
	for (uint y = 0; y < T::NumRows; y++)
	{
		for (uint x = 0; x < T::NumCols; x++)
		{
			Result[y][x] = InMatrix[x][y];
		}
	}
	return Result;
}

// Forward is expected to be normalized already
inline matrix matView(float3 Eye, float3 Forward, float3 Up)
{
	const float3 ZAxis = normalized(Eye - Forward);
	const float3 XAxis = normalized(cross(Up, ZAxis));
	const float3 YAxis = cross(ZAxis, XAxis);

	matrix Result(
		float4(XAxis.x, XAxis.y, XAxis.z, -dot(XAxis, Eye)),
		float4(YAxis.x, YAxis.y, YAxis.z, -dot(YAxis, Eye)),
		float4(ZAxis.x, ZAxis.y, ZAxis.z, -dot(ZAxis, Eye)),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	return Result;
}

inline matrix matTranslation(const float3& Position)
{
	matrix Result(1.0f);
	Result[0][3] = Position.x;
	Result[1][3] = Position.y;
	Result[2][3] = Position.z;
	return Result;
}

inline matrix matRotation(const Quaternion& Rotation)
{
	return Rotation.RotationMatrix();
}

inline matrix matScale(const float3& Scale)
{
	matrix Result(1.0f);

	Result[0][0] = Scale.x;
	Result[1][1] = Scale.y;
	Result[2][2] = Scale.z;

	return Result;
}

inline matrix matTransformation(const float3& Position, const Quaternion& Rotation, const float3& Scale)
{
	matrix Result;

	const matrix m1 = matTranslation(Position);
	const matrix m2 = matRotation(Rotation);
	const matrix m3 = matScale(Scale);

	Result = m1 * m2 * m3;

	return Result;
}




#endif //__BANE_MATH_H__
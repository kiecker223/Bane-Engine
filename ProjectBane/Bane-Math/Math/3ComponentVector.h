#pragma once

#include "Common/Types.h"
#include "2ComponentVector.h"
#include <xmmintrin.h>
#include <emmintrin.h>

struct ivec3
{
	using TT = ivec3;
	using T = int;
	using TT2 = ivec2;
	static const uint32 ColCount = 3;

	union
	{
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { T u, v, w; };
		TT2 rg;
		TT2 xy;
		TT2 uv;
		T p[ColCount];
	};
	
	ivec3() :
		x(0),
		y(0),
		z(0)
	{
	}

	ivec3(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	ivec3(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	ivec3(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	ivec3(TT2 XY, T Z) :
		x(XY.x),
		y(XY.y),
		z(Z)
	{
	}



	inline TT& operator = (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
		}
		return *this;
	}
	inline T& operator[] (const uint32 Index)
	{
		return p[Index];
	}

	inline const T operator[] (const uint32 Index) const
	{
		return p[Index];
	}

#pragma region Operators

#pragma region Self modifyable operators

	inline TT& operator *= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] += Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator *= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs;
		}
		return *this;
	}

	inline TT& operator /= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs;
		}
		return *this;
	}

	inline TT& operator += (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] += Rhs;
		}
		return *this;
	}

	inline TT& operator -= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs;
		}
		return *this;
	}

#pragma endregion

#pragma region Non self modifying operators

	inline TT operator-() const
	{
		return TT(-x, -y, -z);
	}

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs.p[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs.p[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs.p[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs.p[i];
		}
		return Result;
	}

	inline TT operator * (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs;
		}
		return Result;
	}

	inline TT operator / (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs;
		}
		return Result;
	}

	inline TT operator + (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs;
		}
		return Result;
	}

	inline TT operator - (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs;
		}
		return Result;
	}

#pragma endregion

#pragma endregion
};

struct uvec3
{
	using T = uint32;
	using TT = uvec3;
	using TT2 = uvec2;
	static const uint32 ColCount = 3;
	uvec3()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	uvec3(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	uvec3(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	uvec3(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	uvec3(TT2 XY, T Z) :
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

	inline TT& operator = (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
		}
		return *this;
	}
	inline T& operator[] (const uint32 Index)
	{
		return p[Index];
	}

	inline const T operator[] (const uint32 Index) const
	{
		return p[Index];
	}

#pragma region Operators

#pragma region Self modifyable operators

	inline TT& operator *= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] += Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator *= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs;
		}
		return *this;
	}

	inline TT& operator /= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs;
		}
		return *this;
	}

	inline TT& operator += (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] += Rhs;
		}
		return *this;
	}

	inline TT& operator -= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs;
		}
		return *this;
	}

#pragma endregion

#pragma region Non self modifying operators

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs.p[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs.p[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs.p[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs.p[i];
		}
		return Result;
	}

	inline TT operator * (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs;
		}
		return Result;
	}

	inline TT operator / (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs;
		}
		return Result;
	}

	inline TT operator + (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs;
		}
		return Result;
	}

	inline TT operator - (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs;
		}
		return Result;
	}

#pragma endregion

#pragma endregion
};

struct fvec3
{
	using T = float;
	using TT = fvec3;
	using TT2 = fvec2;
	static const uint32 ColCount = 3;

	union
	{
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { T u, v, w; };
		TT2 rg;
		TT2 xy;
		T p[ColCount];
	};

	fvec3()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	fvec3(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	fvec3(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	fvec3(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	fvec3(TT2 XY, T Z) :
		x(XY.x),
		y(XY.y),
		z(Z)
	{
	}

	inline TT& operator = (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
		}
		return *this;
	}

	inline constexpr T& operator[](const uint32 Index)
	{
		return p[Index];
	}

	inline constexpr const T operator[](const uint32 Index) const
	{
		return p[Index];
	}
#pragma region Operators

#pragma region Self modifyable operators

	inline TT& operator *= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] += Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator *= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] *= Rhs;
		}
		return *this;
	}

	inline TT& operator /= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs;
		}
		return *this;
	}

	inline TT& operator += (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] += Rhs;
		}
		return *this;
	}

	inline TT& operator -= (const T& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs;
		}
		return *this;
	}

#pragma endregion

#pragma region Non self modifying operators

	inline TT operator-() const
	{
		return TT(-x, -y, -z);
	}

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs.p[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs.p[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs.p[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs.p[i];
		}
		return Result;
	}

	inline TT operator * (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs;
		}
		return Result;
	}

	inline TT operator / (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs;
		}
		return Result;
	}

	inline TT operator + (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs;
		}
		return Result;
	}

	inline TT operator - (const T& Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs;
		}
		return Result;
	}

#pragma endregion

#pragma endregion
};

struct vec3
{
	using TT = vec3;
	using T = double;
	using TT2 = vec2;
	static const uint32 ColCount = 3;

	union
	{
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { T u, v, w; };
		TT2 rg;
		TT2 xy;
		TT2 uv;
		T p[ColCount];
	};

	vec3() :
		x(0),
		y(0),
		z(0)
	{
	}

	vec3(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	vec3(const TT& SelfType)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		*pSelf = _mm_load_pd(reinterpret_cast<const double*>(&SelfType));
		z = SelfType.z;
	}

	vec3(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	vec3(TT2 XY, T Z) :
		x(XY.x),
		y(XY.y),
		z(Z)
	{
	}



	inline TT& operator = (const TT& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		*pSelf = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		z = Rhs.z;
		return *this;
	}
	inline T& operator[] (const uint32 Index)
	{
		return p[Index];
	}

	inline const T operator[] (const uint32 Index) const
	{
		return p[Index];
	}

#pragma region Operators

#pragma region Self modifyable operators

	inline TT& operator *= (const TT& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		__m128d Other = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		*pSelf = _mm_mul_pd(*pSelf, Other);
		z *= Rhs.z;
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		__m128d Other = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		*pSelf = _mm_div_pd(*pSelf, Other);
		z /= Rhs.z;
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		__m128d Other = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		*pSelf = _mm_add_pd(*pSelf, Other);
		z += Rhs.z;
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		__m128d Other = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		*pSelf = _mm_sub_pd(*pSelf, Other);
		y -= Rhs.y;
		return *this;
	}

	inline TT& operator *= (const T& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		__m128d Other = _mm_load1_pd(&Rhs);
		*pSelf = _mm_mul_pd(*pSelf, Other);
		z *= Rhs;
		return *this;
	}

	inline TT& operator /= (const T& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		__m128d Other = _mm_load1_pd(&Rhs);
		*pSelf = _mm_div_pd(*pSelf, Other);
		z /= Rhs;
		return *this;
	}

	inline TT& operator += (const T& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		__m128d Other = _mm_load1_pd(&Rhs);
		*pSelf = _mm_add_pd(*pSelf, Other);
		z += Rhs;
		return *this;
	}

	inline TT& operator -= (const T& Rhs)
	{
		__m128d* pSelf = reinterpret_cast<__m128d*>(this);
		__m128d Other = _mm_load1_pd(&Rhs);
		*pSelf = _mm_sub_pd(*pSelf, Other);
		z -= Rhs;
		return *this;
	}

#pragma endregion

#pragma region Non self modifying operators

	inline TT operator-() const
	{
		return TT(-x, -y, -z);
	}

	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		__m128d* mResult = reinterpret_cast<__m128d*>(&Result);
		__m128d Self = _mm_load_pd(reinterpret_cast<const double*>(this));
		__m128d Other = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		*mResult = _mm_mul_pd(Self, Other);
		Result.z = z * Rhs.z;
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		__m128d* mResult = reinterpret_cast<__m128d*>(&Result);
		__m128d Self = _mm_load_pd(reinterpret_cast<const double*>(this));
		__m128d Other = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		*mResult = _mm_div_pd(Self, Other);
		Result.z = z / Rhs.z;
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		__m128d* mResult = reinterpret_cast<__m128d*>(&Result);
		__m128d Self = _mm_load_pd(reinterpret_cast<const double*>(this));
		__m128d Other = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		*mResult = _mm_add_pd(Self, Other);
		Result.z = z + Rhs.z;
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		__m128d* mResult = reinterpret_cast<__m128d*>(&Result);
		__m128d Self = _mm_load_pd(reinterpret_cast<const double*>(this));
		__m128d Other = _mm_load_pd(reinterpret_cast<const double*>(&Rhs));
		*mResult = _mm_sub_pd(Self, Other);
		Result.z = z - Rhs.z;
		return Result;
	}

	inline TT operator * (const T& Rhs) const
	{
		TT Result;
		__m128d* mResult = reinterpret_cast<__m128d*>(&Result);
		__m128d Self = _mm_load_pd(reinterpret_cast<const double*>(this));
		__m128d Other = _mm_load1_pd(reinterpret_cast<const double*>(&Rhs));
		*mResult = _mm_mul_pd(Self, Other);
		Result.z = z * Rhs;
		return Result;
	}

	inline TT operator / (const T& Rhs) const
	{
		TT Result;
		__m128d* mResult = reinterpret_cast<__m128d*>(&Result);
		__m128d Self = _mm_load_pd(reinterpret_cast<const double*>(this));
		__m128d Other = _mm_load1_pd(reinterpret_cast<const double*>(&Rhs));
		*mResult = _mm_div_pd(Self, Other);
		Result.z = z / Rhs;
		return Result;
	}

	inline TT operator + (const T& Rhs) const
	{
		TT Result;
		__m128d* mResult = reinterpret_cast<__m128d*>(&Result);
		__m128d Self = _mm_load_pd(reinterpret_cast<const double*>(this));
		__m128d Other = _mm_load1_pd(reinterpret_cast<const double*>(&Rhs));
		*mResult = _mm_add_pd(Self, Other);
		Result.z = z + Rhs;
		return Result;
	}

	inline TT operator - (const T& Rhs) const
	{
		TT Result;
		__m128d* mResult = reinterpret_cast<__m128d*>(&Result);
		__m128d Self = _mm_load_pd(reinterpret_cast<const double*>(this));
		__m128d Other = _mm_load1_pd(reinterpret_cast<const double*>(&Rhs));
		*mResult = _mm_sub_pd(Self, Other);
		Result.z = z - Rhs;
		return Result;
	}

#pragma endregion

#pragma endregion
};
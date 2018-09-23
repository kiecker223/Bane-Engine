#pragma once

#include "Common/Types.h"
#include "2ComponentVector.h"

struct int3
{
	using TT = int3;
	using T = int;
	using TT2 = int2;
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
	
	int3() :
		x(0),
		y(0),
		z(0)
	{
	}

	int3(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	int3(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	int3(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	int3(TT2 XY, T Z) :
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

struct uint323
{
	using T = uint32;
	using TT = uint323;
	using TT2 = uint322;
	static const uint32 ColCount = 3;
	uint323()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	uint323(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	uint323(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	uint323(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	uint323(TT2 XY, T Z) :
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

struct float3
{
	using T = float;
	using TT = float3;
	using TT2 = float2;
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

	float3()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	float3(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	float3(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	float3(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	float3(TT2 XY, T Z) :
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

struct double3
{
	using TT = double3;
	using T = double;
	using TT2 = double2;
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

	double3() :
		x(0),
		y(0),
		z(0)
	{
	}

	double3(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{
	}

	double3(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	double3(T X, TT2 YZ) :
		x(X),
		y(YZ.x),
		z(YZ.y)
	{
	}

	double3(TT2 XY, T Z) :
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
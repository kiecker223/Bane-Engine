#pragma once

#include "Types.h"

struct int2
{
	using T = int;
	using TT = int2;
	static const uint ColCount = 2;
	
	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
		T p[ColCount];
	};
	
	int2()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}

	int2(T X, T Y) :
		x(X),
		y(Y)
	{
	}

	int2(const int2& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}
	inline TT& operator = (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
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
			p[i] *= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs.p[i];
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
			Result[i] = p[i] * Rhs.p[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs.p[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs.p[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs.p[i];
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

#pragma endregion

#pragma endregion
};

struct uint2
{
	using T = uint;
	using TT = uint2;
	static const uint ColCount = 2;
	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
		T p[ColCount];
	};

	uint2()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	
	uint2(T X, T Y) :
		x(X),
		y(Y)
	{
	}

	uint2(const TT& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	inline TT& operator = (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
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
			p[i] *= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs.p[i];
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



	inline TT operator * (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] * Rhs.p[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs.p[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs.p[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs.p[i];
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

#pragma endregion

#pragma endregion
};

struct float2
{
	using T = float;
	using TT = float2;
	static const uint ColCount = 2;
	
	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
		T p[ColCount];
	};

	float2()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	
	float2(T X, T Y) :
		x(X),
		y(Y)
	{
	}

	float2(const TT& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}
	inline TT& operator = (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
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
			p[i] *= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator /= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] /= Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator += (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] += Rhs.p[i];
		}
		return *this;
	}

	inline TT& operator -= (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] -= Rhs.p[i];
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
			Result[i] = p[i] * Rhs.p[i];
		}
		return Result;
	}

	inline TT operator / (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] / Rhs.p[i];
		}
		return Result;
	}

	inline TT operator + (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] + Rhs.p[i];
		}
		return Result;
	}

	inline TT operator - (const TT& Rhs) const
	{
		TT Result;
		for (uint i = 0; i < ColCount; i++)
		{
			Result[i] = p[i] - Rhs.p[i];
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

#pragma endregion

#pragma endregion
};

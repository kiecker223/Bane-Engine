#pragma once

#include <Common/Types.h>
#include <math.h>

struct ivec2
{
	using T = int;
	using TT = ivec2;
	static const uint32 ColCount = 2;
	
	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
		T p[ColCount];
	};
	
	ivec2()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}

	ivec2(T X, T Y) :
		x(X),
		y(Y)
	{
	}

	ivec2(const ivec2& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
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
		return TT(-x, -y);
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

struct uvec2
{
	using T = uint32;
	using TT = uvec2;
	static const uint32 ColCount = 2;
	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
		T p[ColCount];
	};

	uvec2()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	
	uvec2(T X, T Y) :
		x(X),
		y(Y)
	{
	}

	uvec2(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
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

struct fvec2
{
	using T = float;
	using TT = fvec2;
	static const uint32 ColCount = 2;
	
	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
		T p[ColCount];
	};

	fvec2()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	
	fvec2(T X, T Y) :
		x(X),
		y(Y)
	{
	}

	fvec2(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
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
		return TT(-x, -y);
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

struct vec2
{
	using T = double;
	using TT = vec2;
	static const uint32 ColCount = 2;

	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
		T p[ColCount];
	};

	vec2()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}

	vec2(T X, T Y) :
		x(X),
		y(Y)
	{
	}

	vec2(const vec2& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
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
		return TT(-x, -y);
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

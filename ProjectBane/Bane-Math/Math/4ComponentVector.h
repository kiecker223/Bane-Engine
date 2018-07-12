#pragma once

#include "3ComponentVector.h"

struct int4
{
	using T = int;
	using TT = int4;
	using TT3 = int3;
	using TT2 = int2;
	static const uint ColCount = 4;

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

	int4()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	int4(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	int4(const TT& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	int4(T X, T Y, TT2 ZW) :
		x(X),
		y(Y),
		z(ZW.x),
		w(ZW.y)
	{
	}

	int4(TT3 XYZ, T W) :
		x(XYZ.x),
		y(XYZ.y),
		z(XYZ.w),
		w(W)
	{
	}
	inline TT& operator = (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
		}
	}

	inline constexpr T& operator[](const uint Index)
	{
		return p[Index];
	}

	inline constexpr const T operator[](const uint Index) const
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
		return TT(-x, -y, -z, -w);
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

struct uint4
{
	using T = uint;
	using TT = uint4;
	using TT3 = uint3;
	using TT2 = uint2;
	static const uint ColCount = 4;

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

	uint4()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}

	uint4(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	uint4(const TT& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	uint4(T X, T Y, TT2 ZW) :
		x(X),
		y(Y),
		z(ZW.x),
		w(ZW.y)
	{
	}

	uint4(TT3 XYZ, T W) :
		x(XYZ.x),
		y(XYZ.y),
		z(XYZ.w),
		w(W)
	{
	}
	inline TT& operator = (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
		}
	}

	inline constexpr T& operator[](const uint Index) 
	{
		return p[Index];
	}

	inline constexpr const T operator[](const uint Index) const
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

struct float4
{
	using T = float;
	using TT = float4;
	using TT3 = float3;
	using TT2 = float2;
	static const uint ColCount = 4;

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

	float4()
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	float4(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	float4(const TT& SelfType)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	float4(T X, T Y, TT2 ZW) :
		x(X),
		y(Y),
		z(ZW.x),
		w(ZW.y)
	{
	}

	float4(TT3 XYZ, T W) :
		x(XYZ.x),
		y(XYZ.y),
		z(XYZ.w),
		w(W)
	{
	}

	inline TT& operator = (const TT& Rhs)
	{
		for (uint i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
		}
		return *this;
	}

	inline constexpr T& operator[](const uint Index)
	{
		return p[Index];
	}

	inline const T operator[](const uint Index) const
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
		return TT(-x, -y, -z, -w);
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

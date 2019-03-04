#pragma once

#include "3ComponentVector.h"

struct ivec4
{
	using T = int;
	using TT = ivec4;
	using TT3 = ivec3;
	using TT2 = ivec2;
	static const uint32 ColCount = 4;

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

	ivec4()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	ivec4(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	ivec4(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	ivec4(T X, T Y, TT2 ZW) :
		x(X),
		y(Y),
		z(ZW.x),
		w(ZW.y)
	{
	}

	ivec4(TT3 XYZ, T W) :
		x(XYZ.x),
		y(XYZ.y),
		z(XYZ.w),
		w(W)
	{
	}
	inline TT& operator = (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
		}
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
		return TT(-x, -y, -z, -w);
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

struct uvec4
{
	using T = uint32;
	using TT = uvec4;
	using TT3 = uvec3;
	using TT2 = uvec2;
	static const uint32 ColCount = 4;

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

	uvec4()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}

	uvec4(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	uvec4(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	uvec4(T X, T Y, TT2 ZW) :
		x(X),
		y(Y),
		z(ZW.x),
		w(ZW.y)
	{
	}

	uvec4(TT3 XYZ, T W) :
		x(XYZ.x),
		y(XYZ.y),
		z(XYZ.w),
		w(W)
	{
	}
	inline TT& operator = (const TT& Rhs)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = Rhs.p[i];
		}
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

struct fvec4
{
	using T = float;
	using TT = fvec4;
	using TT3 = fvec3;
	using TT2 = fvec2;
	static const uint32 ColCount = 4;

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

	fvec4()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	fvec4(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	fvec4(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	fvec4(T X, T Y, TT2 ZW) :
		x(X),
		y(Y),
		z(ZW.x),
		w(ZW.y)
	{
	}

	fvec4(TT3 XYZ, T W) :
		x(XYZ.x),
		y(XYZ.y),
		z(XYZ.w),
		w(W)
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

	inline const T operator[](const uint32 Index) const
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
		return TT(-x, -y, -z, -w);
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

struct vec4
{
	using T = double;
	using TT = vec4;
	using TT3 = vec3;
	using TT2 = vec2;
	static const uint32 ColCount = 4;

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

	vec4()
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = T(0);
		}
	}
	vec4(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{
	}

	vec4(const TT& SelfType)
	{
		for (uint32 i = 0; i < ColCount; i++)
		{
			p[i] = SelfType[i];
		}
	}

	vec4(T X, T Y, TT2 ZW) :
		x(X),
		y(Y),
		z(ZW.x),
		w(ZW.y)
	{
	}

	vec4(TT3 XYZ, T W) :
		x(XYZ.x),
		y(XYZ.y),
		z(XYZ.w),
		w(W)
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
		return TT(-x, -y, -z, -w);
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


inline fvec4 fromDouble4(const vec4& InVal)
{
	return fvec4(static_cast<float>(InVal.x), static_cast<float>(InVal.y), static_cast<float>(InVal.z), static_cast<float>(InVal.w));
}

inline fvec3 fromDouble3(const vec3& InVal)
{
	return fvec3(static_cast<float>(InVal.x), static_cast<float>(InVal.y), static_cast<float>(InVal.z));
}

inline fvec2 fromDouble2(const vec2& InVal)
{
	return fvec2(static_cast<float>(InVal.x), static_cast<float>(InVal.y));
}

inline vec4 fromFloat4(const fvec4& InVal)
{
	return vec4(static_cast<double>(InVal.x), static_cast<double>(InVal.y), static_cast<double>(InVal.z), static_cast<double>(InVal.w));
}

inline vec3 fromFloat3(const fvec3& InVal)
{
	return vec3(static_cast<double>(InVal.x), static_cast<double>(InVal.y), static_cast<double>(InVal.z));
}

inline vec2 fromFloat2(const fvec2& InVal)
{
	return vec2(static_cast<double>(InVal.x), static_cast<double>(InVal.y));
}
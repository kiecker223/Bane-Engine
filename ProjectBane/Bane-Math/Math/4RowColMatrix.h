#pragma once

#include "3RowColMatrix.h"
#include "4ComponentVector.h"
#include <xmmintrin.h>

struct imat4x4
{
	using T = int;
	using TT = imat4x4;
	using TT3 = imat3x3;
	using TT2 = imat2x2;
	using RowType = ivec4;
	static const uint32 NumRows = 4;
	static const uint32 NumCols = 4;

	union
	{
		T p[4][4];
		RowType v[4];
	};

	imat4x4()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	imat4x4(T IdentityVal)
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

	imat4x4(const imat4x4& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	imat4x4(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	imat4x4(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	imat4x4(RowType r1, RowType r2, RowType r3, RowType r4)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
		v[3] = r4;
	}

	inline constexpr RowType& operator[](uint32 Index)
	{
		return v[Index];
	}

	inline RowType operator[](uint32 Index) const
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
		Result.p[0][0] = p[0][0] * Rhs.p[0][0] + p[0][1] * Rhs.p[1][0] + p[0][2] * Rhs.p[2][0] + p[0][3] * Rhs.p[3][0];
		Result.p[0][1] = p[0][0] * Rhs.p[0][1] + p[0][1] * Rhs.p[1][1] + p[0][2] * Rhs.p[2][1] + p[0][3] * Rhs.p[3][1];
		Result.p[0][2] = p[0][0] * Rhs.p[0][2] + p[0][1] * Rhs.p[1][2] + p[0][2] * Rhs.p[2][2] + p[0][3] * Rhs.p[3][2];
		Result.p[0][3] = p[0][0] * Rhs.p[0][3] + p[0][1] * Rhs.p[1][3] + p[0][2] * Rhs.p[2][3] + p[0][3] * Rhs.p[3][3];
		Result.p[1][0] = p[1][0] * Rhs.p[0][0] + p[1][1] * Rhs.p[1][0] + p[1][2] * Rhs.p[2][0] + p[1][3] * Rhs.p[3][0];
		Result.p[1][1] = p[1][0] * Rhs.p[0][1] + p[1][1] * Rhs.p[1][1] + p[1][2] * Rhs.p[2][1] + p[1][3] * Rhs.p[3][1];
		Result.p[1][2] = p[1][0] * Rhs.p[0][2] + p[1][1] * Rhs.p[1][2] + p[1][2] * Rhs.p[2][2] + p[1][3] * Rhs.p[3][2];
		Result.p[1][3] = p[1][0] * Rhs.p[0][3] + p[1][1] * Rhs.p[1][3] + p[1][2] * Rhs.p[2][3] + p[1][3] * Rhs.p[3][3];
		Result.p[2][0] = p[2][0] * Rhs.p[0][0] + p[2][1] * Rhs.p[1][0] + p[2][2] * Rhs.p[2][0] + p[2][3] * Rhs.p[3][0];
		Result.p[2][1] = p[2][0] * Rhs.p[0][1] + p[2][1] * Rhs.p[1][1] + p[2][2] * Rhs.p[2][1] + p[2][3] * Rhs.p[3][1];
		Result.p[2][2] = p[2][0] * Rhs.p[0][2] + p[2][1] * Rhs.p[1][2] + p[2][2] * Rhs.p[2][2] + p[2][3] * Rhs.p[3][2];
		Result.p[2][3] = p[2][0] * Rhs.p[0][3] + p[2][1] * Rhs.p[1][3] + p[2][2] * Rhs.p[2][3] + p[2][3] * Rhs.p[3][3];
		Result.p[3][0] = p[3][0] * Rhs.p[0][0] + p[3][1] * Rhs.p[1][0] + p[3][2] * Rhs.p[2][0] + p[3][3] * Rhs.p[3][0];
		Result.p[3][1] = p[3][0] * Rhs.p[0][1] + p[3][1] * Rhs.p[1][1] + p[3][2] * Rhs.p[2][1] + p[3][3] * Rhs.p[3][1];
		Result.p[3][2] = p[3][0] * Rhs.p[0][2] + p[3][1] * Rhs.p[1][2] + p[3][2] * Rhs.p[2][2] + p[3][3] * Rhs.p[3][2];
		Result.p[3][3] = p[3][0] * Rhs.p[0][3] + p[3][1] * Rhs.p[1][3] + p[3][2] * Rhs.p[2][3] + p[3][3] * Rhs.p[3][3];
		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		p[0][0] = p[0][0] * Rhs.p[0][0] + p[0][1] * Rhs.p[1][0] + p[0][2] * Rhs.p[2][0] + p[0][3] * Rhs.p[3][0];
		p[0][1] = p[0][0] * Rhs.p[0][1] + p[0][1] * Rhs.p[1][1] + p[0][2] * Rhs.p[2][1] + p[0][3] * Rhs.p[3][1];
		p[0][2] = p[0][0] * Rhs.p[0][2] + p[0][1] * Rhs.p[1][2] + p[0][2] * Rhs.p[2][2] + p[0][3] * Rhs.p[3][2];
		p[0][3] = p[0][0] * Rhs.p[0][3] + p[0][1] * Rhs.p[1][3] + p[0][2] * Rhs.p[2][3] + p[0][3] * Rhs.p[3][3];
		p[1][0] = p[1][0] * Rhs.p[0][0] + p[1][1] * Rhs.p[1][0] + p[1][2] * Rhs.p[2][0] + p[1][3] * Rhs.p[3][0];
		p[1][1] = p[1][0] * Rhs.p[0][1] + p[1][1] * Rhs.p[1][1] + p[1][2] * Rhs.p[2][1] + p[1][3] * Rhs.p[3][1];
		p[1][2] = p[1][0] * Rhs.p[0][2] + p[1][1] * Rhs.p[1][2] + p[1][2] * Rhs.p[2][2] + p[1][3] * Rhs.p[3][2];
		p[1][3] = p[1][0] * Rhs.p[0][3] + p[1][1] * Rhs.p[1][3] + p[1][2] * Rhs.p[2][3] + p[1][3] * Rhs.p[3][3];
		p[2][0] = p[2][0] * Rhs.p[0][0] + p[2][1] * Rhs.p[1][0] + p[2][2] * Rhs.p[2][0] + p[2][3] * Rhs.p[3][0];
		p[2][1] = p[2][0] * Rhs.p[0][1] + p[2][1] * Rhs.p[1][1] + p[2][2] * Rhs.p[2][1] + p[2][3] * Rhs.p[3][1];
		p[2][2] = p[2][0] * Rhs.p[0][2] + p[2][1] * Rhs.p[1][2] + p[2][2] * Rhs.p[2][2] + p[2][3] * Rhs.p[3][2];
		p[2][3] = p[2][0] * Rhs.p[0][3] + p[2][1] * Rhs.p[1][3] + p[2][2] * Rhs.p[2][3] + p[2][3] * Rhs.p[3][3];
		p[3][0] = p[3][0] * Rhs.p[0][0] + p[3][1] * Rhs.p[1][0] + p[3][2] * Rhs.p[2][0] + p[3][3] * Rhs.p[3][0];
		p[3][1] = p[3][0] * Rhs.p[0][1] + p[3][1] * Rhs.p[1][1] + p[3][2] * Rhs.p[2][1] + p[3][3] * Rhs.p[3][1];
		p[3][2] = p[3][0] * Rhs.p[0][2] + p[3][1] * Rhs.p[1][2] + p[3][2] * Rhs.p[2][2] + p[3][3] * Rhs.p[3][2];
		p[3][3] = p[3][0] * Rhs.p[0][3] + p[3][1] * Rhs.p[1][3] + p[3][2] * Rhs.p[2][3] + p[3][3] * Rhs.p[3][3];
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

	inline TT& operator *= (T Rhs)
	{
		for (uint32 i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (T Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < NumRows; i++)
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

struct umat4x4
{
	using T = uint32;
	using TT = umat4x4;
	using TT3 = umat3x3;
	using TT2 = umat2x2;
	using RowType = uvec4;
	static const uint32 NumRows = 4;
	static const uint32 NumCols = 4;

	union
	{
		T p[4][4];
		RowType v[4];
	};

	umat4x4()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	umat4x4(T IdentityVal)
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

	umat4x4(const umat4x4& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	umat4x4(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	umat4x4(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	umat4x4(RowType r1, RowType r2, RowType r3, RowType r4)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
		v[3] = r4;
	}

	inline constexpr RowType& operator[](uint32 Index)
	{
		return v[Index];
	}

	inline RowType operator[](uint32 Index) const
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

	inline TT& operator *= (T Rhs)
	{
		for (uint32 i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (T Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < NumRows; i++)
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

struct fmat4x4
{
	using T = float;
	using TT = fmat4x4;
	using TT3 = fmat3x3;
	using TT2 = fmat2x2;
	using RowType = fvec4;
	static const uint32 NumRows = 4;
	static const uint32 NumCols = 4;

	union
	{
		__m128 f[4];
		T p[4][4];
		RowType v[4];
	};

	fmat4x4()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	fmat4x4(T IdentityVal)
	{
		float Val = 0.0f;
		for (int i = 0; i < NumRows; i++)
		{
			f[i] = _mm_load1_ps(&Val);
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
		p[3][3] = IdentityVal;
	}

	fmat4x4(const fmat4x4& Rhs)
	{
		f[0] = _mm_load_ps(Rhs.p[0]);
		f[1] = _mm_load_ps(Rhs.p[1]);
		f[2] = _mm_load_ps(Rhs.p[2]);
		f[3] = _mm_load_ps(Rhs.p[3]);
	}

	fmat4x4(const T* Values)
	{
		f[0] = _mm_load_ps(&Values[0]);
		f[1] = _mm_load_ps(&Values[3]);
		f[2] = _mm_load_ps(&Values[7]);
		f[3] = _mm_load_ps(&Values[11]);
	}

	fmat4x4(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	fmat4x4(RowType r1, RowType r2, RowType r3, RowType r4)
	{
		f[0] = _mm_load_ps(reinterpret_cast<float*>(&r1));
		f[1] = _mm_load_ps(reinterpret_cast<float*>(&r2));
		f[2] = _mm_load_ps(reinterpret_cast<float*>(&r3));
		f[3] = _mm_load_ps(reinterpret_cast<float*>(&r4));
	}

	inline constexpr RowType& operator[](uint32 Index)
	{
		return v[Index];
	}

	inline RowType operator[](uint32 Index) const
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
		const __m128 BCx = _mm_load_ps((float*)&Rhs.f[0]);
		const __m128 BCy = _mm_load_ps((float*)&Rhs.f[1]);
		const __m128 BCz = _mm_load_ps((float*)&Rhs.f[2]);
		const __m128 BCw = _mm_load_ps((float*)&Rhs.f[3]);

		float* LeftRowPointer = const_cast<float*>(&p[0][0]);
		float* ResultRowPointer = static_cast<float*>(&Result.p[0][0]);

		for (unsigned int i = 0; i < 4; ++i, LeftRowPointer += 4, ResultRowPointer += 4) 
		{
			__m128 ARx = _mm_set1_ps(LeftRowPointer[0]);
			__m128 ARy = _mm_set1_ps(LeftRowPointer[1]);
			__m128 ARz = _mm_set1_ps(LeftRowPointer[2]);
			__m128 ARw = _mm_set1_ps(LeftRowPointer[3]);

			__m128 X = _mm_mul_ps(ARx, BCx);
			__m128 Y = _mm_mul_ps(ARy, BCy);
			__m128 Z = _mm_mul_ps(ARz, BCz);
			__m128 W = _mm_mul_ps(ARw, BCw);

			__m128 R = _mm_add_ps(X, _mm_add_ps(Y, _mm_add_ps(Z, W)));
			_mm_store_ps(ResultRowPointer, R);
		}
		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		const __m128 BCx = _mm_load_ps((float*)&Rhs.f[0]);
		const __m128 BCy = _mm_load_ps((float*)&Rhs.f[1]);
		const __m128 BCz = _mm_load_ps((float*)&Rhs.f[2]);
		const __m128 BCw = _mm_load_ps((float*)&Rhs.f[3]);

		float* LeftRowPointer = const_cast<float*>(&p[0][0]);
		float* ResultRowPointer = static_cast<float*>(&p[0][0]);

		for (unsigned int i = 0; i < 4; ++i, LeftRowPointer += 4, ResultRowPointer += 4)
		{
			__m128 ARx = _mm_set1_ps(LeftRowPointer[0]);
			__m128 ARy = _mm_set1_ps(LeftRowPointer[1]);
			__m128 ARz = _mm_set1_ps(LeftRowPointer[2]);
			__m128 ARw = _mm_set1_ps(LeftRowPointer[3]);

			__m128 X = _mm_mul_ps(ARx, BCx);
			__m128 Y = _mm_mul_ps(ARy, BCy);
			__m128 Z = _mm_mul_ps(ARz, BCz);
			__m128 W = _mm_mul_ps(ARw, BCw);

			__m128 R = _mm_add_ps(X, _mm_add_ps(Y, _mm_add_ps(Z, W)));
			_mm_store_ps(ResultRowPointer, R);
		}
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
		for (uint32 i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (float Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < NumRows; i++)
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

typedef fmat4x4 matrix;


struct mat4x4
{
	using T = double;
	using TT = mat4x4;
	using TT3 = mat3x3;
	using TT2 = mat2x2;
	using RowType = vec4;
	static const uint32 NumRows = 4;
	static const uint32 NumCols = 4;

	union
	{
		T p[4][4];
		RowType v[4];
	};

	mat4x4()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	mat4x4(T IdentityVal)
	{
		for (uint32 i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
		p[3][3] = IdentityVal;
	}

	mat4x4(const mat4x4& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	mat4x4(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	mat4x4(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	mat4x4(RowType r1, RowType r2, RowType r3, RowType r4)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
		v[3] = r4;
	}

	inline constexpr RowType& operator[](uint32 Index)
	{
		return v[Index];
	}

	inline RowType operator[](uint32 Index) const
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

	inline TT& operator *= (T Rhs)
	{
		for (uint32 i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (T Rhs) const
	{
		TT Result;
		for (uint32 i = 0; i < NumRows; i++)
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
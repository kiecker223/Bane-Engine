#pragma once

#include "3RowColMatrix.h"
#include "4ComponentVector.h"

struct int4x4
{
	using T = int;
	using TT = int4x4;
	using TT3 = int3x3;
	using TT2 = int2x2;
	using RowType = int4;
	static const uint32 NumRows = 4;
	static const uint32 NumCols = 4;

	union
	{
		T p[4][4];
		RowType v[4];
	};

	int4x4()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	int4x4(T IdentityVal)
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

	int4x4(const int4x4& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	int4x4(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	int4x4(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	int4x4(RowType r1, RowType r2, RowType r3, RowType r4)
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

struct uint324x4
{
	using T = uint32;
	using TT = uint324x4;
	using TT3 = uint323x3;
	using TT2 = uint322x2;
	using RowType = uint324;
	static const uint32 NumRows = 4;
	static const uint32 NumCols = 4;

	union
	{
		T p[4][4];
		RowType v[4];
	};

	uint324x4()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	uint324x4(T IdentityVal)
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

	uint324x4(const uint324x4& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	uint324x4(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	uint324x4(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	uint324x4(RowType r1, RowType r2, RowType r3, RowType r4)
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

struct float4x4
{
	using T = float;
	using TT = float4x4;
	using TT3 = float3x3;
	using TT2 = float2x2;
	using RowType = float4;
	static const uint32 NumRows = 4;
	static const uint32 NumCols = 4;

	union
	{
		T p[4][4];
		RowType v[4];
	};

	float4x4()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	float4x4(T IdentityVal)
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

	float4x4(const float4x4& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	float4x4(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	float4x4(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	float4x4(RowType r1, RowType r2, RowType r3, RowType r4)
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

typedef float4x4 matrix;

/*
struct double4x4
{
	using T = double;
	using TT = double4x4;
	using TT3 = double3x3;
	using TT2 = double2x2;
	using RowType = double4;
	static const uint32 NumRows = 4;
	static const uint32 NumCols = 4;

	union
	{
		T p[4][4];
		RowType v[4];
	};

	double4x4()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
		v[3] = RowType();
	}

	double4x4(T IdentityVal)
	{
		for (double i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
		p[3][3] = IdentityVal;
	}

	double4x4(const double4x4& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	double4x4(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	double4x4(T p00, T p01, T p02, T p03, T p10, T p11, T p12, T p13, T p20, T p21, T p22, T p23, T p30, T p31, T p32, T p33)
	{
		v[0] = RowType(p00, p01, p02, p03);
		v[1] = RowType(p10, p11, p12, p13);
		v[2] = RowType(p20, p21, p22, p23);
		v[3] = RowType(p30, p31, p32, p33);
	}

	double4x4(RowType r1, RowType r2, RowType r3, RowType r4)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
		v[3] = r4;
	}

	inline constexpr RowType& operator[](udouble32 Index)
	{
		return v[Index];
	}

	inline RowType operator[](udouble32 Index) const
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
		for (udouble32 i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (T Rhs) const
	{
		TT Result;
		for (udouble32 i = 0; i < NumRows; i++)
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
*/
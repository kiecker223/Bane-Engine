#pragma once

#include "2ComponentVector.h"

struct int2x2
{
	using TT = int2x2;
	using T = int;
	using RowType = int2;
	static const uint NumRows = 2;
	static const uint NumCols = 2;

	union
	{
		T p[2][2];
		RowType v[2];
	};

	int2x2()
	{
		v[0] = RowType();
		v[1] = RowType();
	}

	int2x2(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
	}

	int2x2(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	int2x2(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	int2x2(T p00, T p01, T p10, T p11)
	{
		v[0] = RowType(p00, p01);
		v[1] = RowType(p10, p11);
	}

	int2x2(RowType r1, RowType r2)
	{
		v[0] = r1;
		v[1] = r2;
	}

	inline constexpr RowType& operator[] (const uint Index)
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

	inline TT& operator *= (T Rhs)
	{
		for (uint i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (T Rhs) const
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

struct uint2x2
{
	using TT = uint2x2;
	using T = uint;
	using RowType = uint2;
	static const uint NumRows = 2;
	static const uint NumCols = 2;

	union
	{
		T p[2][2];
		RowType v[2];
	};

	uint2x2()
	{
		v[0] = RowType();
		v[1] = RowType();
	}

	uint2x2(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
	}

	uint2x2(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	uint2x2(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	uint2x2(T p00, T p01, T p10, T p11)
	{
		v[0] = RowType(p00, p01);
		v[1] = RowType(p10, p11);
	}

	uint2x2(RowType r1, RowType r2)
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

	inline TT& operator *= (T Rhs)
	{
		for (uint i = 0; i < NumRows; i++)
		{
			v[i] *= Rhs;
		}
		return *this;
	}

	inline TT operator * (T Rhs) const
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

struct float2x2
{
	using TT = float2x2;
	using T = float;
	using RowType = float2;
	static const uint NumRows = 2;
	static const uint NumCols = 2;

	union
	{
		T p[2][2];
		RowType v[2];
	};

	float2x2()
	{
		v[0] = RowType();
		v[1] = RowType();
	}

	float2x2(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
	}

	float2x2(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	float2x2(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	float2x2(T p00, T p01, T p10, T p11)
	{
		v[0] = RowType(p00, p01);
		v[1] = RowType(p10, p11);
	}

	float2x2(RowType r1, RowType r2)
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


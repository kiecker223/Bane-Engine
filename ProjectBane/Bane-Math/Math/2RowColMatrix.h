#pragma once

#include "2ComponentVector.h"

struct imat2x2
{
	using TT = imat2x2;
	using T = int;
	using RowType = ivec2;
	static const uint32 NumRows = 2;
	static const uint32 NumCols = 2;

	union
	{
		T p[2][2];
		RowType v[2];
	};

	imat2x2()
	{
		v[0] = RowType();
		v[1] = RowType();
	}

	imat2x2(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
	}

	imat2x2(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	imat2x2(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	imat2x2(T p00, T p01, T p10, T p11)
	{
		v[0] = RowType(p00, p01);
		v[1] = RowType(p10, p11);
	}

	imat2x2(RowType r1, RowType r2)
	{
		v[0] = r1;
		v[1] = r2;
	}

	inline constexpr RowType& operator[] (const uint32 Index)
	{
		return v[Index];
	}

	inline const RowType operator[] (const uint32 Index) const
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

#pragma endregion
};

struct umat2x2
{
	using TT = umat2x2;
	using T = uint32;
	using RowType = uvec2;
	static const uint32 NumRows = 2;
	static const uint32 NumCols = 2;

	union
	{
		T p[2][2];
		RowType v[2];
	};

	umat2x2()
	{
		v[0] = RowType();
		v[1] = RowType();
	}

	umat2x2(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
	}

	umat2x2(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	umat2x2(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	umat2x2(T p00, T p01, T p10, T p11)
	{
		v[0] = RowType(p00, p01);
		v[1] = RowType(p10, p11);
	}

	umat2x2(RowType r1, RowType r2)
	{
		v[0] = r1;
		v[1] = r2;
	}

	inline RowType& operator[] (const uint32 Index)
	{
		return v[Index];
	}

	inline const RowType operator[] (const uint32 Index) const
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

#pragma endregion
};

struct fmat2x2
{
	using TT = fmat2x2;
	using T = float;
	using RowType = fvec2;
	static const uint32 NumRows = 2;
	static const uint32 NumCols = 2;

	union
	{
		T p[2][2];
		RowType v[2];
	};

	fmat2x2()
	{
		v[0] = RowType();
		v[1] = RowType();
	}

	fmat2x2(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
	}

	fmat2x2(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	fmat2x2(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	fmat2x2(T p00, T p01, T p10, T p11)
	{
		v[0] = RowType(p00, p01);
		v[1] = RowType(p10, p11);
	}

	fmat2x2(RowType r1, RowType r2)
	{
		v[0] = r1;
		v[1] = r2;
	}

	inline RowType& operator[] (const uint32 Index)
	{
		return v[Index];
	}

	inline const RowType operator[] (const uint32 Index) const
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

#pragma endregion
};

struct mat2x2
{
	using TT = mat2x2;
	using T = double;
	using RowType = vec2;
	static const uint32 NumRows = 2;
	static const uint32 NumCols = 2;

	union
	{
		T p[2][2];
		RowType v[2];
	};

	mat2x2()
	{
		v[0] = RowType();
		v[1] = RowType();
	}

	mat2x2(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
	}

	mat2x2(const TT& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	mat2x2(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	mat2x2(T p00, T p01, T p10, T p11)
	{
		v[0] = RowType(p00, p01);
		v[1] = RowType(p10, p11);
	}

	mat2x2(RowType r1, RowType r2)
	{
		v[0] = r1;
		v[1] = r2;
	}

	inline constexpr RowType& operator[] (const uint32 Index)
	{
		return v[Index];
	}

	inline const RowType operator[] (const uint32 Index) const
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

#pragma endregion
};
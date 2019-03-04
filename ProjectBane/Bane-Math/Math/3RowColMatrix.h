#pragma once

#include "2RowColMatrix.h"
#include "3ComponentVector.h"

struct imat3x3
{
	using TT = imat3x3;
	using TT2 = imat2x2;
	using T = int;
	using RowType = ivec3;
	static const uint32 NumRows = 3;
	static const uint32 NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	imat3x3()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	imat3x3(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	imat3x3(const imat3x3& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	imat3x3(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	imat3x3(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	imat3x3(RowType r1, RowType r2, RowType r3)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
	}

	inline constexpr RowType& operator[](const uint32 Index)
	{
		return v[Index];
	}

	inline const RowType operator[](const uint32 Index) const
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
		Result[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		Result[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		Result[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		Result[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		Result[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		Result[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		Result[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		Result[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		Result[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		p[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		p[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		p[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		p[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		p[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		p[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		p[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		p[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		p[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return *this;
	}

	inline RowType operator * (const RowType& Rhs) const
	{
		RowType Result;
		Result.x = Rhs.x * p[0][0] + Rhs.y * p[0][1] + Rhs.z * p[0][2];
		Result.y = Rhs.x * p[1][0] + Rhs.y * p[1][1] + Rhs.z * p[1][2];
		Result.z = Rhs.x * p[2][0] + Rhs.y * p[2][1] + Rhs.z * p[2][2];
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

	inline operator TT2() const
	{
		return TT2(
			p[0][0], p[0][1],
			p[1][0], p[1][1]
		);
	}

#pragma endregion
};

struct umat3x3
{
	using TT = umat3x3;
	using TT2 = umat2x2;
	using T = uint32;
	using RowType = uvec3;
	static const uint32 NumRows = 3;
	static const uint32 NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	umat3x3()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	umat3x3(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	umat3x3(const umat3x3& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	umat3x3(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	umat3x3(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	umat3x3(RowType r1, RowType r2, RowType r3)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
	}

	inline constexpr RowType& operator[](const uint32 Index)
	{
		return v[Index];
	}

	inline const RowType operator[](const uint32 Index) const
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
		Result[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		Result[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		Result[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		Result[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		Result[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		Result[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		Result[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		Result[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		Result[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		p[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		p[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		p[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		p[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		p[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		p[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		p[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		p[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		p[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return *this;
	}

	inline RowType operator * (const RowType& Rhs) const
	{
		RowType Result;
		Result.x = Rhs.x * p[0][0] + Rhs.y * p[0][1] + Rhs.z * p[0][2];
		Result.y = Rhs.x * p[1][0] + Rhs.y * p[1][1] + Rhs.z * p[1][2];
		Result.z = Rhs.x * p[2][0] + Rhs.y * p[2][1] + Rhs.z * p[2][2];
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

	inline operator TT2() const
	{
		return TT2(
			p[0][0], p[0][1],
			p[1][0], p[1][1]
		);
	}

#pragma endregion
};

struct fmat3x3
{
	using TT = fmat3x3;
	using TT2 = fmat2x2;
	using T = float;
	using RowType = fvec3;
	static const uint32 NumRows = 3;
	static const uint32 NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	fmat3x3()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	fmat3x3(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	fmat3x3(const fmat3x3& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	fmat3x3(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	fmat3x3(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	fmat3x3(RowType r1, RowType r2, RowType r3)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
	}

	inline constexpr RowType& operator[](const uint32 Index)
	{
		return v[Index];
	}

	inline const RowType operator[](const uint32 Index) const
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
		Result[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		Result[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		Result[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		Result[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		Result[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		Result[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		Result[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		Result[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		Result[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		p[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		p[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		p[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		p[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		p[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		p[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		p[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		p[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		p[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return *this;
	}

	inline RowType operator * (const RowType& Rhs) const
	{
		RowType Result;
		Result.x = Rhs.x * p[0][0] + Rhs.y * p[0][1] + Rhs.z * p[0][2];
		Result.y = Rhs.x * p[1][0] + Rhs.y * p[1][1] + Rhs.z * p[1][2];
		Result.z = Rhs.x * p[2][0] + Rhs.y * p[2][1] + Rhs.z * p[2][2];
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

	inline operator TT2() const
	{
		return TT2(
			p[0][0], p[0][1],
			p[1][0], p[1][1]
		);
	}

#pragma endregion
};

struct mat3x3
{
	using TT =  mat3x3;
	using TT2 = mat2x2;
	using T = double;
	using RowType = vec3;
	static const uint32 NumRows = 3;
	static const uint32 NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	mat3x3()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	mat3x3(T IdentityVal)
	{
		for (uint32 i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	mat3x3(const mat3x3& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	mat3x3(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	mat3x3(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	mat3x3(RowType r1, RowType r2, RowType r3)
	{
		v[0] = r1;
		v[1] = r2;
		v[2] = r3;
	}

	inline constexpr RowType& operator[](const uint32 Index)
	{
		return v[Index];
	}

	inline const RowType operator[](const uint32 Index) const
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
		Result[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		Result[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		Result[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		Result[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		Result[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		Result[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		Result[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		Result[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		Result[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return Result;
	}

	inline TT& operator *= (const TT& Rhs)
	{
		p[0][0] = p[0][0] * Rhs[0][0] + p[0][1] * Rhs[1][0] + p[0][2] * Rhs[2][0];
		p[0][1] = p[0][0] * Rhs[0][1] + p[0][1] * Rhs[1][1] + p[0][2] * Rhs[2][1];
		p[0][2] = p[0][0] * Rhs[0][2] + p[0][1] * Rhs[1][2] + p[0][2] * Rhs[2][2];

		p[1][0] = p[1][0] * Rhs[0][0] + p[1][1] * Rhs[1][0] + p[1][2] * Rhs[2][0];
		p[1][1] = p[1][0] * Rhs[0][1] + p[1][1] * Rhs[1][1] + p[1][2] * Rhs[2][1];
		p[1][2] = p[1][0] * Rhs[0][2] + p[1][1] * Rhs[1][2] + p[1][2] * Rhs[2][2];

		p[2][0] = p[2][0] * Rhs[0][0] + p[2][1] * Rhs[1][0] + p[2][2] * Rhs[2][0];
		p[2][1] = p[2][0] * Rhs[0][1] + p[2][1] * Rhs[1][1] + p[2][2] * Rhs[2][1];
		p[2][2] = p[2][0] * Rhs[0][2] + p[2][1] * Rhs[1][2] + p[2][2] * Rhs[2][2];

		return *this;
	}

	inline RowType operator * (const RowType& Rhs) const
	{
		RowType Result;
		Result.x = Rhs.x * p[0][0] + Rhs.y * p[0][1] + Rhs.z * p[0][2];
		Result.y = Rhs.x * p[1][0] + Rhs.y * p[1][1] + Rhs.z * p[1][2];
		Result.z = Rhs.x * p[2][0] + Rhs.y * p[2][1] + Rhs.z * p[2][2];
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

	inline operator TT2() const
	{
		return TT2(
			p[0][0], p[0][1],
			p[1][0], p[1][1]
		);
	}

#pragma endregion
};


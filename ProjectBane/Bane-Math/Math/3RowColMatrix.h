#pragma once

#include "2RowColMatrix.h"
#include "3ComponentVector.h"

struct int3x3
{
	using TT = int3x3;
	using TT2 = int2x2;
	using T = int;
	using RowType = int3;
	static const uint32 NumRows = 3;
	static const uint32 NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	int3x3()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	int3x3(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	int3x3(const int3x3& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	int3x3(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	int3x3(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	int3x3(RowType r1, RowType r2, RowType r3)
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

struct uint323x3
{
	using TT = uint323x3;
	using TT2 = uint322x2;
	using T = uint32;
	using RowType = uint3;
	static const uint32 NumRows = 3;
	static const uint32 NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	uint323x3()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	uint323x3(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	uint323x3(const uint323x3& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	uint323x3(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	uint323x3(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	uint323x3(RowType r1, RowType r2, RowType r3)
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

struct float3x3
{
	using TT = float3x3;
	using TT2 = float2x2;
	using T = float;
	using RowType = float3;
	static const uint32 NumRows = 3;
	static const uint32 NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	float3x3()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	float3x3(T IdentityVal)
	{
		for (int i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	float3x3(const float3x3& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	float3x3(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	float3x3(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	float3x3(RowType r1, RowType r2, RowType r3)
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

struct double3x3
{
	using TT =  double3x3;
	using TT2 = double2x2;
	using T = double;
	using RowType = double3;
	static const uint32 NumRows = 3;
	static const uint32 NumCols = 3;

	union
	{
		T p[3][3];
		RowType v[3];
	};

	double3x3()
	{
		v[0] = RowType();
		v[1] = RowType();
		v[2] = RowType();
	}

	double3x3(T IdentityVal)
	{
		for (uint32 i = 0; i < NumRows; i++)
		{
			v[i] = RowType();
		}
		p[0][0] = IdentityVal;
		p[1][1] = IdentityVal;
		p[2][2] = IdentityVal;
	}

	double3x3(const double3x3& Rhs)
	{
		memcpy((void*)this, (void*)&Rhs, sizeof(*this));
	}

	double3x3(const T* Values)
	{
		memcpy(p, Values, sizeof(p));
	}

	double3x3(T p00, T p01, T p02, T p10, T p11, T p12, T p20, T p21, T p22)
	{
		v[0] = RowType(p00, p01, p02);
		v[1] = RowType(p10, p11, p12);
		v[2] = RowType(p20, p21, p22);
	}

	double3x3(RowType r1, RowType r2, RowType r3)
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


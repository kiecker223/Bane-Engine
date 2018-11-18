#pragma once

#include <KieckerMath.h>
#include <numeric>

class BoundingBox
{
public:

	BoundingBox()
	{
	}

	BoundingBox(const BoundingBox& Rhs) : Min(Rhs.Min), Max(Rhs.Max) { }

	BoundingBox(double3 InMin, double3 InMax) : Min(InMin), Max(InMax) { }

	BoundingBox& operator = (const BoundingBox& Rhs)
	{
		Min = Rhs.Min;
		Max = Rhs.Max;
		return *this;
	}

	double3 Min;
	double3 Max;

	inline double3 GetExtents() const
	{
		return Max - Min;
	}

	inline uint32 GetBiggestAxis() const
	{
		double3 Extents = GetExtents();
		if (abs(Extents.x) > abs(Extents.y) && abs(Extents.x) > abs(Extents.z))
		{
			return 0;
		}
		if (abs(Extents.y) > abs(Extents.x) && abs(Extents.y) > abs(Extents.z))
		{
			return 1;
		}
		if (abs(Extents.z) > abs(Extents.x) && abs(Extents.z) > abs(Extents.y))
		{
			return 2;
		}
		return 3;
	}

	inline double3 GetCenter() const 
	{
		return Min + (GetExtents() / 2.0);
	}

	inline double GetArea() const 
	{
		auto Extents = GetExtents();
		return (Extents.x * Extents.y * Extents.z);
	}

	inline bool AllComponentsLess(const double3& Lhs, const double3& Rhs)
	{
		return Lhs.x < Rhs.x && Lhs.y < Rhs.y && Lhs.z < Rhs.z;
	}

	inline bool AllComponentsGreater(const double3& Lhs, const double3& Rhs)
	{
		return !AllComponentsLess(Lhs, Rhs);
	}

	inline void CalculateFromArray(const double3* pPoints, uint32 ArraySize)
	{
		double3 NewMin(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
		double3 NewMax(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
		for (uint32 i = 0; i < ArraySize; i++)
		{
			if (pPoints[i].x < NewMin.x) NewMin.x = pPoints[i].x;
			if (pPoints[i].y < NewMin.y) NewMin.y = pPoints[i].y;
			if (pPoints[i].z < NewMin.z) NewMin.z = pPoints[i].z;

			if (pPoints[i].x > NewMax.x) NewMax.x = pPoints[i].x;
			if (pPoints[i].y > NewMax.y) NewMax.y = pPoints[i].y;
			if (pPoints[i].z > NewMax.z) NewMax.z = pPoints[i].z;
		}
		Min = NewMin;
		Max = NewMax;
	}

};

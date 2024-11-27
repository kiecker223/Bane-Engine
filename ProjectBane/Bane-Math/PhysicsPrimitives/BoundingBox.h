#pragma once

#include "../BaneMath.h"
#include <numeric>

class BoundingBox
{
public:

	BoundingBox()
	{
	}

	BoundingBox(const BoundingBox& Rhs) : Min(Rhs.Min), Max(Rhs.Max) { }

	BoundingBox(vec3 InMin, vec3 InMax) : Min(InMin), Max(InMax) { }

	BoundingBox& operator = (const BoundingBox& Rhs)
	{
		Min = Rhs.Min;
		Max = Rhs.Max;
		return *this;
	}

	vec3 Min;
	vec3 Max;

	inline vec3 GetExtents() const
	{
		return Max - Min;
	}

	inline uint32 GetBiggestAxis() const
	{
		vec3 Extents = GetExtents();
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

	inline bool PointInBounds(const vec3& Point) const
	{
		for (uint32 i = 0; i < 3; i++)
		{
			if (Point[i] < Min[i] || Point[i] > Max[i])
			{
				return false;
			}
		}
		return true;
	}

	inline bool IntersectsOther(const BoundingBox& Rhs) const
	{
		for (uint32 i = 0; i < 3; i++)
		{
			if (Max[i] < Rhs.Min[i] || Min[i] > Rhs.Max[i]) return false;
		}
		return true;
	}

	inline vec3 GetCenter() const 
	{
		return Min + (GetExtents() / 2.0);
	}

	inline vec3 ClosestPoint(const vec3& InPoint) const
	{
		vec3 Result;
		for (uint32 i = 0; i < 3; i++)
		{
			if (InPoint[i] > Max[i]) { Result[i] = Max[i]; continue; }
			if (InPoint[i] < Min[i]) { Result[i] = Min[i]; continue; }
			Result[i] = InPoint[i];
		}
		return Result;
	}
	
	inline double GetArea() const 
	{
		auto Extents = GetExtents();
		return (Extents.x * Extents.y * Extents.z);
	}

	inline bool AllComponentsLess(const vec3& Lhs, const vec3& Rhs)
	{
		return Lhs.x < Rhs.x && Lhs.y < Rhs.y && Lhs.z < Rhs.z;
	}

	inline bool AllComponentsGreater(const vec3& Lhs, const vec3& Rhs)
	{
		return !AllComponentsLess(Lhs, Rhs);
	}

	inline BoundingBox Subdivide(uint32 SubdivideRegion)
	{
		BoundingBox Result(*this);
		vec3 Center = GetCenter();
		switch (SubdivideRegion)
		{
		case 0:
			Result.Min.yz = Center.yz;
			Result.Max.x = Center.x;
			break;
		case 1:
			Result.Min = Center;
			break;
		case 2:
			Result.Min.xy = Center.xy;
			Result.Max.z = Center.z;
			break;
		case 3:
			Result.Min.y = Center.y;
			Result.Max.x = Center.x;
			Result.Max.z = Center.z;
			break;
		case 4:
			Result.Min.z = Center.z;
			Result.Max.xy = Center.xy;
			break;
		case 5:
			Result.Min.x = Center.x;
			Result.Min.z = Center.z;
			Result.Max.y = Center.y;
			break;
		case 6:
			Result.Min.x = Center.x;
			Result.Max.yz = Center.yz;
			break;
		case 7:
			Result.Max = Center;
			break;
		}
		return Result;
	}

	inline void CalculateFromArray(const vec3* pPoints, uint32 ArraySize)
	{
		vec3 NewMin(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
		vec3 NewMax(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
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

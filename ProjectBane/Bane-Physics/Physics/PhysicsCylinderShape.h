#pragma once

#include <KieckerMath.h>

class PhysicsCylinderShape
{
public:
	
	double Height;
	double Radius;

	inline double TestRayHit(const double3& RayStart, const double3& RayDirection, const Quaternion& Orientation, double3& HitNormal) const
	{
		UNUSED(Orientation);
		double a = RayStart.x * RayStart.x + RayStart.z * RayStart.z;
		double b = 2. * RayStart.x * RayStart.x + 2. * RayStart.z * RayStart.z;
		double c = RayStart.x * RayStart.x + RayStart.z * RayStart.z - 1.;

		double SqrtPart = b * b + (4 * (a*c));
		if (SqrtPart < 0)
		{
			return -1.;
		}
		SqrtPart = sqrt(SqrtPart);
		
		double ResultA = (-b + SqrtPart) / 2 * a;
		double ResultB = (-b - SqrtPart) / 2 * a;

		if (ResultA > ResultB)
		{
			std::swap(ResultA, ResultB);
		}

		double y0 = RayStart.y + ResultA * RayDirection.y;
		double y1 = RayStart.y + ResultB * RayDirection.y;

		double HalfHeight = Height / 2.0;
		if (y0 < -HalfHeight)
		{ 
			if (y1 < -HalfHeight)
			{
				return -1;
			}
			else
			{
				double Th = ResultA + (ResultA - ResultB) * (ResultA + 1.) / (ResultA - ResultB);
				if (Th <= 0.) { return -1.; }
				HitNormal = double3(0., -1., 0.);
				return Th;
			}

		}
		else if (y0 >= -HalfHeight && y0 <= HalfHeight)
		{
			if (ResultA <= 0.) { return -1.; }
			double3 HitPos = RayStart + (RayDirection * ResultA);
			HitNormal = normalized(double3(HitPos.x, 0, HitPos.z));
			return ResultA;
		}
		else if (y0 > HalfHeight)
		{
			if (y1 > HalfHeight) { return -1.; }
			double Th = ResultA + (ResultB - ResultA) * (ResultB - 1) / (ResultA - ResultB);
			if (Th <= 0.) { return -1.; }
			HitNormal = double3(0., -1., 0.);
			return Th;
		}
		return -1.;
	}
};

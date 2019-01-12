#include "PhysicsBody.h"
#include <iostream>

void PhysicsBody::Accelerate(double3 Direction, double Magnitude)
{
	Velocity += (Direction * Magnitude);
}

static bool AllComponentsLess(const double3& Lhs, const double3& Rhs)
{
	return (Lhs.x < Rhs.x) && (Lhs.y < Rhs.y) && (Lhs.z < Rhs.z);
}

double PhysicsBody::FindClosestApproach(const PhysicsBody& OtherBody, double& OutT) const
{
	double ResultingT = 0.0;
	FindClosestTimeOfApproach(OtherBody, ResultingT);
	OutT = ResultingT;
	double Result = 0.0;

	double OtherTime = 0.0;
	OtherBody.FindClosestTimeOfApproach(*this, OtherTime);

	double3 OtherClosestApproach = OtherBody.Position + (normalized(OtherBody.Velocity) * OtherTime);

	Result = length(OtherClosestApproach - Position + (Velocity * ResultingT));
	return Result;
}

void PhysicsBody::FindClosestTimeOfApproach(const PhysicsBody& OtherBody, double& OutT) const
{
	double3 a;
	if (length(Velocity) > 1e-4)
	{
		a = normalized(Velocity);
	}
	double3 b;
	if (length(OtherBody.Velocity) > 1e-4)
	{
		b = normalized(OtherBody.Velocity);
	}
	double3 c = OtherBody.Position - Position;

	double Dab = dot(a, b);

	if (Dab == 1.)
	{
		double Result = length(c);
		OutT = Result / 2.;
	}

	double Dbc = dot(b, c);
	double Dbb = dot(b, b);
	double Daa = dot(a, a);
	double Dac = dot(a, c);

	double ResultingT = (-(Dab * Dbc) + (Dac * Dbb)) / ((Daa * Dbb) - (Dab * Dab));

	if (AllComponentsLess(Abs(Position + Velocity) - Abs(Position + (a * ResultingT)), double3(0., 0., 0.)))
	{
		__debugbreak();
		ResultingT = -1;
	}

	OutT = ResultingT;
}


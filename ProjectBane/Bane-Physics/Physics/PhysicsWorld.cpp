#include "PhysicsWorld.h"



// Gravitational constant  G = (6.6747*10^-11)
// Gravitational equation F = G*((m1*m2)/r*r)


void PhysicsWorld::UpdateBodies()
{
	for (auto& Body : Bodies)
	{
		for (auto& OtherBody : Bodies)
		{
			if (OtherBody.Handle == Body.Handle)
			{
				continue;
			}
			double3 ForceDir = OtherBody.Position - Body.Position;
			double DistanceFromBody = length(ForceDir);
			normalize(ForceDir);

			double Force = M_GRAV_CONST * ((Body.Mass * OtherBody.Mass) / (DistanceFromBody * DistanceFromBody));
			ForceDir *= Force;
			double3 AccelerationDir = ForceDir / Body.Mass;
			Body.Velocity += ((AccelerationDir * 1. / 60.));
			if (length(Body.Velocity) > 0.0001)
				Body.Position += Body.Velocity;
		}
	}
	UpdateBuffer.Bodies = Bodies;
}
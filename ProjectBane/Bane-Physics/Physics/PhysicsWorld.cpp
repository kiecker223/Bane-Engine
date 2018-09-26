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
			double3 AccelerationDir = OtherBody.Position - Body.Position;
			double DistanceFromBody = length(AccelerationDir);
			normalize(AccelerationDir);

			double Force = M_GRAV_CONST * ((Body.Mass * OtherBody.Mass) / DistanceFromBody * DistanceFromBody);
			AccelerationDir *= Force;
			if (isnan(AccelerationDir.x))
			{
				__debugbreak();
			}
			Body.Velocity += AccelerationDir;
			if (length(Body.Velocity) > 0.0001)
				Body.Position += Body.Velocity;
		}
	}
	UpdateBuffer.Bodies = Bodies;
}
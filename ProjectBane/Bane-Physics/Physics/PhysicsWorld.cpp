#include "PhysicsWorld.h"



// Gravitational constant  G = (6.6747*10^-11)
// Gravitational equation F = G*((m1*m2)/r*r)
// 

#define GRAV_CONST 6.6747E-11f

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
			float3 AccelerationDir = OtherBody.Position - Body.Position;
			float DistanceFromBody = length(AccelerationDir);
			normalize(AccelerationDir);

			float Force = GRAV_CONST * ((Body.Mass * OtherBody.Mass) / DistanceFromBody * DistanceFromBody);
			AccelerationDir *= Force;
			Body.Velocity += AccelerationDir;
			Body.Position += Body.Velocity;
		}
	}
	UpdateBuffer.Bodies = Bodies;
}
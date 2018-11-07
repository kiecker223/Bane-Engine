#include "PhysicsBody.h"


void PhysicsBody::Accelerate(double3 Direction, double Magnitude)
{
	UNUSED(Direction);
	UNUSED(Magnitude);
	Velocity += (Direction * Magnitude);
}

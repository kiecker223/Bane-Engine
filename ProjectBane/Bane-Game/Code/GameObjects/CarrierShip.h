#pragma once

#include "PhysicsPrimitives/CollisionPrimitives.h"
#include "PhysicsData.h"

class IGraphicsCommandContext;

namespace nlohmann
{
	struct json;
}

class CarrierShip
{
public:

	CarrierShip();

	CollisionShape& CollisionShapeRef;
	PhysicsData& DataRef;

	void Accelerate(const vec3& Acceleration);

	void Draw(IGraphicsCommandContext* Context);
};

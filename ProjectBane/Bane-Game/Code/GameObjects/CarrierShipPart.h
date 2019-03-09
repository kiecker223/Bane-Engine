#pragma once

#include "PhysicsPrimitives/CollisionPrimitives.h"
#include "CombatShipPart.h"

class CarrierShipPart
{
public:

	~CarrierShipPart()
	{

	}

	Mesh* pMesh;

	vec3 LocalPosition;
	Quaternion LocalRotation;
	vec3 LocalScale;

	matrix LocalTransformation;

	CollisionShape LocalShape;

	// Use this to 
	inline void SetTransformations(const vec3& InLocalPosition, const Quaternion& InLocalRotation, const vec3& InLocalScale)
	{
		LocalPosition = InLocalPosition;
		LocalRotation = InLocalRotation;
		LocalScale = InLocalScale;
		LocalTransformation = matTransformation(fromDouble3(LocalPosition), InLocalRotation, fromDouble3(LocalScale));
	}

	virtual void OnDisable() = 0;
	virtual void OnDamageTaken(EDAMAGE_LEVEL DamageLevel) = 0;
	virtual uint32 StartRepairs() = 0;

};

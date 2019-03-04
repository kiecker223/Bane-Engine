#pragma once

#include "CombatShipPart.h"

class CarrierShipPart
{
public:

	Mesh* pMesh;

	vec3 LocalPosition;
	vec3 Scale;

	virtual void OnDisable() = 0;
	virtual void OnDamageTaken(EDAMAGE_LEVEL DamageLevel) = 0;
	virtual uint32 StartRepairs() = 0;

};

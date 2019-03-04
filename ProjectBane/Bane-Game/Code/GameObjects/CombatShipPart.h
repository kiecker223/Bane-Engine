#pragma once

#include <GraphicsCore/Data/Mesh.h>

typedef enum EDAMAGE_LEVEL
{
	DAMAGE_LEVEL_CRITICAL = 1,
	DAMAGE_LEVEL_MODERATE = 2,
	DAMAGE_LEVEL_INSIGNIFICANT = 3
} EDAMAGE_LEVEL;


class CombatShipPart
{
public:

	Mesh* pMesh;

	vec3 LocalPosition;
	vec3 Scale;

	virtual void OnDisable() = 0;
	virtual void OnDamageTaken(EDAMAGE_LEVEL DamageLevel) = 0;
	virtual uint32 StartRepairs() = 0;

};

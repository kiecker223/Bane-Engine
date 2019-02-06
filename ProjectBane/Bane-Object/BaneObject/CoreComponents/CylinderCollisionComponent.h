#pragma once

#include "../Entity/Entity.h"
#include <Physics/PhysicsBody.h>

class CylinderCollisionComponent : public Component
{
	IMPLEMENT_COMPONENT(CylinderCollisionComponent)

public:

	void Awake() override final;

	void Tick(double DT) override final;

	void PhysicsTick() override final;

	void SetPosition(const double3& NewPosition);

	void SetVelocity(const double3& NewVelocity);

	void SetOrientation(const Quaternion& NewOrientation);

	void SetHeight(double NewHeight);

	void SetMass(double NewMass);

	// Updated each physics tick (1/60th) of a second
	double3 GetVelocity() const;

private:

	uint32 m_BodyId;
	double3 m_Velocity;

};
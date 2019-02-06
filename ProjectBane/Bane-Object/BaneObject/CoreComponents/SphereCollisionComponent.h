#pragma once

#include "../Entity/Entity.h"
#include <Physics/PhysicsBody.h>


class SphereCollisionComponent : public Component
{
	IMPLEMENT_COMPONENT(SphereCollisionComponent)
public:

	void Awake() final override;

	void Start() final override;

	void Tick(double DT) final override;

	void PhysicsTick() final override;

	void SetVelocity(const double3& NewVelocity);

	void SetAngularVelocity(const double4& AngularVelocity);

	void SetRadius(double Radius);

	void SetPosition(const double3& Position);

	void SetMass(double Mass);

	inline uint32 GetBodyId() const
	{
		return m_Body;
	}

	/*
	@returns: The velocity of the body this represents.
	@note: Updated every physics tick, and returns the value of this the last physics tick
	*/
	inline double3 GetVelocity() const
	{
		return m_Velocity;
	}

	/*
	@returns: The angular velocity of the body this represents.
	@note: Updated every physics tick, returns the value of the last physics tick
	*/
	double4 GetAngularVelocity() const
	{
		return m_AngularVelocity;
	}

	/*
	@returns: The position of the physics body
	@note: Updated every physics tick
	*/
	inline double3 GetPosition() const
	{
		return m_Position;
	}

	inline double GetRadius() const
	{
		return GetTransform()->GetScale().x / 2.;
	}

	inline double GetMass() const
	{
		return m_Mass;
	}

private:
	uint32 m_Body;
	double3 m_Velocity;
	double3 m_Position;
	double4 m_AngularVelocity;
	double m_Mass;
	double m_Radius;
};

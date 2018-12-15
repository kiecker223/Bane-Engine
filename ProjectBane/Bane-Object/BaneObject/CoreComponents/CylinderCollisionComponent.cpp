#include "CylinderCollisionComponent.h"
#include "../Scene/Scene.h"


void CylinderCollisionComponent::Awake()
{
	PHYSICS_BODY_CREATE_INFO CreateInfo;
	CreateInfo.BodyType = PHYSICS_BODY_TYPE_CYLINDER;
	CreateInfo.EntityHandle = GetOwner()->GetId().HashedName;
	m_BodyId = GetScene()->GetPhysicsWorld().AddBody(CreateInfo);
}

void CylinderCollisionComponent::Tick(float DT)
{
	UNUSED(DT);
	auto& WorldSim = GetScene()->GetPhysicsWorld();
	if (WorldSim.IsReadyForRead())
	{
		PhysicsBody& Body = WorldSim.UpdateBuffer.Bodies[m_BodyId];
		GetTransform()->SetPosition(Body.Position);
		m_Velocity = Body.Velocity;
		//GetTransform()->SetRotation(Body.Orientation);
	}
}

void CylinderCollisionComponent::SetPosition(const double3& NewPosition)
{
	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[NewPosition](PhysicsBody& Val)
			{
				Val.Position = NewPosition;
			},
			m_BodyId
		}
	);
}

void CylinderCollisionComponent::SetVelocity(const double3& NewVelocity)
{

	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[NewVelocity](PhysicsBody& Val)
			{
				Val.Velocity = NewVelocity;
			},
			m_BodyId
		}
	);
}

void CylinderCollisionComponent::SetOrientation(const Quaternion& NewOrientation)
{

	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[NewOrientation](PhysicsBody& Val)
			{
				Val.Orientation = NewOrientation;
			},
			m_BodyId
		}
	);
}

void CylinderCollisionComponent::SetHeight(double NewHeight)
{

	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[NewHeight](PhysicsBody& Val)
			{
				Val.Cylinder.Height = NewHeight;
			},
			m_BodyId
		}
	);
}

void CylinderCollisionComponent::SetMass(double NewMass)
{
	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[NewMass](PhysicsBody& Val)
			{
				Val.Mass = NewMass;
			},
			m_BodyId
		}
	);
}

double3 CylinderCollisionComponent::GetVelocity() const
{
	return m_Velocity;
}

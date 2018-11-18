#include "SphereCollisionComponent.h"
#include "../Scene/Scene.h"


void SphereCollisionComponent::Awake()
{
	PHYSICS_BODY_CREATE_INFO CreateInfo = { };
	CreateInfo.BodyType = PHYSICS_BODY_TYPE_SPHERE;
	CreateInfo.EntityHandle = GetOwner()->GetId().HashedName;
	m_Body = GetScene()->GetPhysicsWorld().AddBody(CreateInfo);
}

void SphereCollisionComponent::Start()
{
}

void SphereCollisionComponent::Tick(float DT)
{
	UNUSED(DT);
	if (GetScene()->GetPhysicsWorld().IsReadyForRead())
	{
		auto& RefBody = GetScene()->GetPhysicsWorld().UpdateBuffer.Bodies[m_Body];
		m_Position = RefBody.Position;
		m_Velocity = RefBody.Velocity;
		m_AngularVelocity = double4(normalized(RefBody.AngularVelocity), length(RefBody.AngularVelocity));
		m_Mass = RefBody.Mass;
		GetTransform()->SetPosition(RefBody.Position);
	}
}

void SphereCollisionComponent::SetVelocity(const double3& NewVelocity)
{
	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[NewVelocity](PhysicsBody& Val)
			{
				Val.Velocity = NewVelocity;
			},
			m_Body
		}
	);
}

void SphereCollisionComponent::SetAngularVelocity(const double4& AngularVelocity)
{
	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[AngularVelocity](PhysicsBody& Val)
			{
				Val.AngularVelocity = AngularVelocity.xyz * AngularVelocity.w;
			},
			m_Body
		}
	);
}

void SphereCollisionComponent::SetRadius(double Radius)
{
	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[Radius](PhysicsBody& Val) 
			{
				Val.Sphere.Radius = Radius; 
			},
			m_Body
		}
	);
}

void SphereCollisionComponent::SetPosition(const double3& Position)
{
	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[Position](PhysicsBody& Val) { Val.Position = Position; },
			m_Body
		}
	);
}

void SphereCollisionComponent::SetMass(double Mass)
{
	GetScene()->GetPhysicsWorld().MessageQueue.AddCallback(
		{
			[Mass](PhysicsBody& Val) { Val.Mass = Mass; },
			m_Body
		}
	);
}

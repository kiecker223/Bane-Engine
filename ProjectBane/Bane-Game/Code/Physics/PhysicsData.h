#pragma once

#include "BaneMath.h"
#include "PhysicsPrimitives/BoundingBox.h"
#include <vector>




class PhysicsProperties
{
public:

	double Mass;

};


class NBodyNode;

class PhysicsData
{
public:

	BoundingBox Bounds;

	vec4 AngularVelocity;
	vec3 Position;
	double Mass;
	vec3 Velocity;
};


class NBodyNode
{
public:

	NBodyNode() : Parent(nullptr), Force(0.0)
	{
		for (uint32 i = 0; i < 8; i++)
		{
			NextNodes[i] = nullptr;
		}
	}

	~NBodyNode() { }

	inline NBodyNode(const NBodyNode& Rhs) : Parent(Rhs.Parent), CenterOfMass(Rhs.CenterOfMass), Force(Rhs.Force), Bounds(Rhs.Bounds), Bodies(Rhs.Bodies)
	{
		for (uint32 i = 0; i < 8; i++)
		{
			NextNodes[i] = Rhs.NextNodes[i];
		}
	}

	NBodyNode* NextNodes[8];
	NBodyNode* Parent;
	uint32 ChildIndex;

	inline double CalculateForce()
	{
		Force = 0.;
		if (Bodies.empty()) { return Force; }

		for (auto* Data : Bodies)
		{
			Force += Data->Mass;
		}
		return Force;
	}

	inline vec3 CalculateCenterOfMass()
	{
		CenterOfMass = vec3();
		if (Bodies.empty()) { return CenterOfMass; }

		double TotalWeight = 0.0;
		vec3 TotalPosition;

		for (auto* Data : Bodies)
		{
			TotalWeight += Data->Mass;
			TotalPosition += Data->Position * Data->Mass;
		}
		TotalPosition /= TotalWeight;
		CenterOfMass = TotalPosition;
		return CenterOfMass;
	}

	vec3 CenterOfMass;
	double Force;
	BoundingBox Bounds;

	std::vector<PhysicsData*> Bodies;
};

typedef PhysicsData CurrentPhysicsData;
typedef PhysicsData NextPhysicsData;

class FuturePhysicsData
{
public:
	std::vector<PhysicsData> NewPhysicsData;
	uint32 DirtyPhysicsDataIndex;
	double DirtyPhysicsDataTime;
};

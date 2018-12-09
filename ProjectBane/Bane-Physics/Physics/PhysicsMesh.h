#pragma once

#include <KieckerMath.h>
#include <vector>
#include <Core/Data/BoundingBox.h>
#include <Graphics/Data/Mesh.h>

class PhysicsMesh
{
public:
	PhysicsMesh() { }
	PhysicsMesh(const PhysicsMesh& Other) :
		Faces(Other.Faces),
		Indices(Other.Indices)
	{
	}

	struct Face
	{
		double3 Points[3];
		double3 Normal;

		inline double TestRayHit(const double3& RayPos, const double3& RayDir, double3& OutNormal)
		{
			UNUSED(RayPos); UNUSED(RayDir); UNUSED(OutNormal);
			return -1.;
		}
	};

	std::vector<Face> Faces;
	std::vector<uint32> Indices;
	BoundingBox Bounds;

	inline PhysicsMesh& operator = (const PhysicsMesh& Rhs)
	{
		Faces = Rhs.Faces;
		Indices = Rhs.Indices;
		return *this;
	}

	inline Face GetFaceOriented(const uint32 FaceIdx, const double3& Position, const Quaternion& Rotation)
	{
		Face Result = Faces[FaceIdx];
		auto RotationMat = Rotation.RotationMatrix3x3Doubles();
		for (uint32 i = 0; i < 3; i++)
		{
			Result.Points[i] = (RotationMat * Result.Points[i]);
			Result.Points[i] += Position;
		}
		Result.Normal = (RotationMat * Result.Normal);
		return Result;
	}

	void CreateFromMesh(const Mesh* pMesh, const float3& Scale);

private:

	void RecalculateBoundingBox();
};

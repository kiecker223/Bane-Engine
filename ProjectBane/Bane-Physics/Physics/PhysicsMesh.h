#pragma once

#include <KieckerMath.h>
#include <vector>
#include <Graphics/Data/Mesh.h>

class PhysicsMesh
{
	struct Face
	{
		double3 Points[3];
		double3 Normal;
	};

	std::vector<Face> Faces;
	std::vector<uint32> Indices;

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
	}

	void CreateFromMesh(const Mesh* pMesh, const float3& Scale);
};

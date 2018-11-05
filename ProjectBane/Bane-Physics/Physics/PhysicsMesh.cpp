#include "PhysicsMesh.h"


void PhysicsMesh::CreateFromMesh(const Mesh* pMesh, const float3& Scale)
{
	Indices = pMesh->GetIndices();
	const TArray<FloatInt>& Data = pMesh->GetVertices();
	uint32 Stride = pMesh->GetLayout().Stride;

	for (uint32 i = 0; i < Indices.GetElementCount() / 3; i += 3)
	{
		Face CurrFace;
		for (uint32 x = 0; x < 3; x++)
		{
			uint32 FaceIndex = Indices[i + x];
			const FloatInt* pData = &Data[Stride * FaceIndex];
			CurrFace.Points[i] = fromFloat3(float3(pData[0].F * Scale.x, pData[1].F * Scale.y, pData[2].F * Scale.z));
		}
		CurrFace.Normal = normalized(cross(CurrFace.Points[1] - CurrFace.Points[0], CurrFace.Points[2] - CurrFace.Points[0]));
		Faces.Add(CurrFace);
	}
}

void PhysicsMesh::RecalculateBoundingBox()
{
	TArray<double3> Positions;

	for (uint32 i = 0; i < Faces.GetElementCount(); i++)
	{
		for (uint32 b = 0; b < 3; b++)
		{
			Positions.Add(Faces[i].Points[b]);
		}
	}

	Bounds.CalculateFromArray(Positions.GetData(), Positions.GetElementCount());
}



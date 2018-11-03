#include "PhysicsMesh.h"



void PhysicsMesh::CreateFromMesh(const Mesh* pMesh, const float3& Scale)
{
	Indices = pMesh->GetIndices();
	const std::vector<FloatInt>& Data = pMesh->GetVertices();
	uint32 Stride = pMesh->GetLayout().Stride;

	for (uint32 i = 0; i < Indices.size() / 3; i += 3)
	{
		Face CurrFace;
		for (uint32 x = 0; x < 3; x++)
		{
			uint32 FaceIndex = Indices[i + x];
			Vertex Vert;
			const FloatInt* pData = &Data[Stride * FaceIndex];
			Vert.Point = fromFloat3(float3(pData[0].F * Scale.x, pData[1].F * Scale.y, pData[2].F * Scale.z));
			Vert.Normal = fromFloat3(float3(pData[3].F * Scale.x, pData[4].F * Scale.y, pData[5].F * Scale.z));
			CurrFace.Vertices[x] = Vert;
		}
		Faces.push_back(CurrFace);
	}
}


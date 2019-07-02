#pragma once

#include <Common.h>
#include <vector>
#include <KieckerMath.h>
#include "GraphicsCore/Interfaces/GraphicsCommandList.h"
#include "GraphicsCore/Interfaces/GraphicsResources.h"
#include "GraphicsCore/Data/Mesh.h"

// Encapsulates the data needed for drawing and representing 
// a physics mesh that can be deformed and destroyed

struct Impact
{
	vec3 Point;
	vec3 Normal;
	vec2 Barycentrics;
	uint32 FaceIndex;
	double Velocity;
};

struct CollisionData
{
	std::vector<Impact> Impacts;
	std::vector<double> TimeToImpacts;
};

struct BSPNode;

class DestructableMesh
{
public:

	struct Properties
	{
		double Thickness;
		double TensileStrength;
	};

	struct Face
	{
	public:
		vec3 Points[3];
		vec3 Normal;

		inline vec3 CalculateNormal()
		{
			Normal = normalized(cross(Points[1] - Points[0], Points[2] - Points[0]));
			return Normal;
		}

		inline bool Intersects(const Face& Rhs, CollisionData& OutData)
		{
			
		}
	};



	void Initialize(Mesh* pMesh);
	bool TestCollision(const DestructableMesh& Other, CollisionData& OutData);

	inline void CheckGraphicsData(IGraphicsCommandContext* pContext) 
	{
		if (bCpuDataDirty)
		{
			SIMDMemcpy(VboUploadData->Map(), CpuRenderingData.data(), static_cast<uint32>(CpuRenderingData.size() * sizeof(Mesh::Vertex)));
			VboUploadData->Unmap();
			pContext->CopyBufferLocations(VboUploadData, 0, VboGpuData, 0, static_cast<uint32>(CpuRenderingData.size() * sizeof(Mesh::Vertex)));
		}

	}

	std::vector<Mesh::Vertex> CpuRenderingData;
	bool bCpuDataDirty;

	std::vector<Face> PhysicsMeshData;
	IBuffer* VboUploadData;
	IBuffer* IboUploadData;
	IBuffer* VboGpuData;
	IBuffer* IboGpuData;
	BSPNode* OwningNode;
};

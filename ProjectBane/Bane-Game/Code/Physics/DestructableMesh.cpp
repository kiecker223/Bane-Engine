#include "DestructableMesh.h"
#include <GraphicsCore/Interfaces/ApiRuntime.h>
#include <string.h>
#include <algorithm>


void DestructableMesh::Initialize(Mesh* pMesh)
{
	bool bHasNormals;
	uint32 NormalsOffset;
	bool bHasPositions;
	uint32 PositionOffset;

	for (auto& LayoutInput : pMesh->GetLayout().Inputs)
	{
		std::string LayoutName = LayoutInput;
		std::transform(LayoutName.begin(), LayoutName.end(), LayoutName.begin(), ::tolower);
		if (LayoutName == "position")
		{
			bHasPositions = true;
			PositionOffset = 0;
		}
		if (LayoutName == "normal")
		{
			bHasNormals = true;
			NormalsOffset = sizeof(fvec3);
		}
	}

	std::vector<vec3> Positions;
	auto& Vertices = pMesh->GetVertices();
	for (uint32 i = 0, x = 0; i < Vertices.size() / 14; i++, x += 14)
	{
		uint32 MemPos = x;
		vec3 Position;
		for (uint32 i = 0; i < 3; i++)
		{
			Position[i] = static_cast<double>(Vertices[MemPos].F);
			MemPos++;
		}
		Positions.push_back(Position);
	}

	auto& Indices = pMesh->GetIndices();
	for (uint32 i = 0; i < Indices.size(); i += 3)
	{
		uint32 i0 = Indices[i + 0];
		uint32 i1 = Indices[i + 1];
		uint32 i2 = Indices[i + 2];

		Face F;
		F.Points[0] = Positions[i0];
		F.Points[1] = Positions[i1];
		F.Points[2] = Positions[i2];
		F.CalculateNormal();
		PhysicsMeshData.push_back(F);
	}

	IGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	CpuRenderingData.resize(pMesh->GetVertices().size() / sizeof(Mesh::Vertex));
	memcpy(CpuRenderingData.data(), pMesh->GetVertices().data(), CpuRenderingData.size() * sizeof(Mesh::Vertex));

	VboGpuData = Device->CreateVertexBuffer(KILOBYTE(32), nullptr);
	IboGpuData = Device->CreateIndexBuffer(KILOBYTE(32), nullptr);

	VboUploadData = Device->CreateStagingBuffer(KILOBYTE(32));
	IboUploadData = Device->CreateStagingBuffer(KILOBYTE(32));
	{
		void* pData = VboUploadData->Map();
		SIMDMemcpy(pData, CpuRenderingData.data(), static_cast<uint32>(CpuRenderingData.size() * sizeof(Mesh::Vertex)));
		VboUploadData->Unmap();
	}
	{
		void* pData = IboUploadData->Map();
		SIMDMemcpy(pData, Indices.data(), static_cast<uint32>(Indices.size()));
		IboUploadData->Unmap();
	}
	IGraphicsCommandContext* Ctx = Device->GetGraphicsContext();
	Ctx->CopyBuffers(VboUploadData, VboGpuData);
	Ctx->CopyBuffers(IboUploadData, IboGpuData);
	Ctx->Flush();
}

bool DestructableMesh::TestCollision(const DestructableMesh& Other, CollisionData& OutData)
{
	for (uint32 i = 0; i < PhysicsMeshData.size(); i++)
	{
		for (uint32 x = 0; x < Other.PhysicsMeshData.size(); x++)
		{

		}
	}
	return false;
}

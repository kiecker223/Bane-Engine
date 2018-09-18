#include "MeshCache.h"
#include <queue>


class MeshAllocator
{
public:

	void Initialize(uint32 NumMeshes)
	{
		BasePointer = new Mesh[NumMeshes];
		for (uint32 i = 0; i < NumMeshes; i++)
		{
			FreeList.push(BasePointer + i);
		}
	}
	
	void Destroy()
	{
		delete BasePointer;
	}

	Mesh* AllocateMesh()
	{
		BANE_CHECK(FreeList.size() > 0);
		Mesh* Result = FreeList.front();
		FreeList.pop();
		return Result;
	}

	void DeallocateMesh(Mesh* pMesh)
	{
		FreeList.push(pMesh);
	}

	Mesh* BasePointer;
	std::queue<Mesh*> FreeList;
};

static MeshAllocator GMeshAllocator;

void MeshCache::Initialize()
{
	GMeshAllocator.Initialize(128);
}

void MeshCache::Destroy()
{
	GMeshAllocator.Destroy();
}

Mesh* MeshCache::AllocateMesh()
{
	return GMeshAllocator.AllocateMesh();
}

void MeshCache::DeallocateMesh(Mesh* pMesh)
{
	GMeshAllocator.DeallocateMesh(pMesh);
}

Mesh* MeshCache::LoadMesh(const std::string& MeshName)
{
	Mesh* Result = AllocateMesh();
	if (Result)
	{
		if (Result->LoadFromFile(MeshName))
			return Result;
	}
	return nullptr;
}

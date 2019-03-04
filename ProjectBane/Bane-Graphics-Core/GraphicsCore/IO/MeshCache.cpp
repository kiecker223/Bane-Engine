#include "MeshCache.h"
#include <Core/Containers/Queue.h>


class MeshAllocator
{
public:

	void Initialize(uint32 NumMeshes)
	{
		BasePointer = new Mesh[NumMeshes]();
		for (uint32 i = 0; i < NumMeshes; i++)
		{
			FreeList.Enqueue(BasePointer + i);
		}
	}
	
	void Destroy()
	{
		delete[] BasePointer;
	}

	Mesh* AllocateMesh()
	{
		return FreeList.Dequeue();
	}

	void DeallocateMesh(Mesh* pMesh)
	{
		FreeList.Enqueue(pMesh);
	}

	Mesh* BasePointer;
	TQueue<Mesh*> FreeList;
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

void MeshCache::SaveMesh(Mesh* pMesh, const std::string& MeshName)
{
	auto Val = m_Meshes.find(MeshName);
	if (Val != m_Meshes.end())
	{
		__debugbreak();
		return;
	}
	m_Meshes.insert(std::pair<std::string, Mesh*>(MeshName, pMesh));
}

Mesh* MeshCache::LoadMesh(const std::string& MeshName)
{
	auto Val = m_Meshes.find(MeshName);
	if (Val == m_Meshes.end())
	{
		Mesh* Result = AllocateMesh();
		if (Result)
		{
			if (Result->LoadFromFile(MeshName))
			{
				m_Meshes.insert(std::pair<std::string, Mesh*>(MeshName, Result));
				return Result;
			}
			return nullptr;
		}
	}
	else
	{
		return Val->second;
	}
	return nullptr;
}

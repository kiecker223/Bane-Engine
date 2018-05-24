#pragma once
#include "Core/Common.h"
#include "../Rendering/Mesh.h"
#include <map>

class MeshCache
{
public:

	static MeshCache* GInstance;

	void Initialize();
	void Destroy();

	//void LoadMesh(const std::string& FileName, std::vector<Mesh>& OutMeshes);

private:


};

ForceInline void InitializeMeshCache()
{
	MeshCache::GInstance = new MeshCache();
	MeshCache::GInstance->Initialize();
}

ForceInline void DestroyMeshCache()
{
	MeshCache::GInstance->Destroy();
	delete MeshCache::GInstance;
}

ForceInline MeshCache* GetMeshCache()
{
	return MeshCache::GInstance;
}


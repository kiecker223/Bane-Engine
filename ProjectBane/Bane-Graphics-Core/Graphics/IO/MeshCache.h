#pragma once
#include "Common.h"
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

inline void InitializeMeshCache()
{
	MeshCache::GInstance = new MeshCache();
	MeshCache::GInstance->Initialize();
}

inline void DestroyMeshCache()
{
	MeshCache::GInstance->Destroy();
	delete MeshCache::GInstance;
}

inline MeshCache* GetMeshCache()
{
	return MeshCache::GInstance;
}


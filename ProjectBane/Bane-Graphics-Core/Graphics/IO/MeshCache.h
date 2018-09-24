#pragma once
#include "Common.h"
#include "../Data/Mesh.h"
#include <map>

class MeshCache
{
public:

	void Initialize();
	void Destroy();

	Mesh* AllocateMesh();
	void DeallocateMesh(Mesh* pMesh);

	Mesh* LoadMesh(const std::string& MeshName);

private:

	std::map<std::string, Mesh*> m_Meshes;

};


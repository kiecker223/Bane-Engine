#pragma once

#include <KieckerMath.h>
#include <vector>
#include <Graphics/Data/Mesh.h>

class PhysicsMesh
{
	struct Vertex
	{
		double3 Point;
		double3 Normal;
	};

	struct Face
	{
		Vertex Vertices[3];
	};

	std::vector<Face> Faces;
	std::vector<uint32> Indices; // Mostly used to draw the debug mesh

	void CreateFromMesh(const Mesh* pMesh, const double3& Scale);

};

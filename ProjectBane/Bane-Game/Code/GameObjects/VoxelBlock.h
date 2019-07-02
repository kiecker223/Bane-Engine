#pragma once

#include <KieckerMath.h>


class VoxelShip;
class IGraphicsCommandContext;

class VoxelBlock
{
	void BuildDestructionMesh();

	void IntersectDestructionMesh();


	ivec3 VoxelLocation;
};
